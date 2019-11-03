
#include "app.h"

#include "mainwindow.h"

#include <QFontDatabase>

#include <QScreen>

static CUTF8TxtWriter m_logger;
ITxtWriter& g_logger(m_logger);

map<E_FontWeight, QFont> g_mapFont;

float g_fPixelRatio = 1;

/*#if __android
static const wstring g_strSdcardPath;

static const wstring& sdcardPath()
{
    if (!g_strSdcardPath.empty())
    {
        return g_strSdcardPath;
    }

    char *pszSdcardPath = getenv("SECONDARY_STORAGE");
    if (NULL == pszSdcardPath)
    {
        pszSdcardPath = getenv("EXTERNAL_STORAGE");
    }
    if (pszSdcardPath)
    {
        return g_strSdcardPath = strutil::fromStr(pszSdcardPath);
    }

    g_strSdcardPath = L"/mnt/sdcard";
    if (!fsutil::existDir(g_strSdcardPath))
    {
        g_strSdcardPath = L"/storage/emulated/0";
        if (!fsutil::existDir(g_strSdcardPath))
        {
            g_strSdcardPath = L"/sdcard";
        }
    }

    return g_strSdcardPath;
}
#endif*/

CApp::CApp(int argc, char **argv) : QApplication(argc, argv)
{
#if __android
    fsutil::setWorkDir(__androidDataDir);
#else
    fsutil::setWorkDir(fsutil::getAppDir());
#endif

    m_logger.open(L"XMusic.log", true);

    QScreen *screen = QApplication::primaryScreen();
    float fPixelRatio = screen->devicePixelRatio();
    auto fDPI = screen->logicalDotsPerInch();
    g_logger << "DPR: " << fPixelRatio << " DPI: " >> fDPI;

#if __ios
    g_fPixelRatio = fPixelRatio;

    /*int nScreenSize = szScreen.width()*szScreen.height();
    switch (nScreenSize)
    {
    case 320*568: // iPhoneSE
        break;
    case 375*667: // iPhone6 iPhone6S iPhone7 iPhone8
        break;
    case 414*736: // iPhone6plus iPhone6Splus iPhone7plus iPhone8plus
        break;
    case 375*812: // iPhoneX iPhoneXS ??
        break;
    case 414*896: // iPhoneXR iPhoneXSmax ??
        break;
    case 1024*1366: // iPadPro1_12.9 iPadPro2_12.9 iPadPro3_12.9
        break;
    case 834*1194: // iPadPro1_11
        break;
    case 834*1112: // iPadPro1_10.5
        break;
    case 768*1024: // iPadPro1_9.7 iPadAir2 iPadAir iPad6 iPad5
        break;
    default:        // iPadMini
        break;
    };*/
#endif

    float fFontSize = 0;
#if __android
    fFontSize = 12;

#elif __ios
    QSize szScreen = screen->size();
    int nScreenWidth = MIN(szScreen.width(), szScreen.height()) ;
    fFontSize = this->font().pointSizeF();
    fFontSize = fFontSize*nScreenWidth/540;

#elif __mac
    fFontSize = 29;

#elif __windows
    float fDPIRate = getDPIRate();
    g_logger << "DPIRate: " >> fDPIRate;

    fFontSize = 22;
    fFontSize *= fDPIRate;
#endif

    SMap<E_FontWeight, QString> mapFontFile {
        { E_FontWeight::FW_Light, "/font/Microsoft-YaHei-Light.ttc" }
        //, { E_FontWeight::FW_Normal, "/font/Microsoft-YaHei-Regular.ttc" }
        , { E_FontWeight::FW_SemiBold, "/font/Microsoft-YaHei-Semibold.ttc" }
    };
    mapFontFile([&](E_FontWeight eWeight, QString qsFontFile) {
#if __android
        qsFontFile = "assets:" +  qsFontFile;
#else
        qsFontFile = strutil::toQstr(fsutil::workDir()) + qsFontFile;
#endif

        QString qsFontfamilyName;
        int fontId = QFontDatabase::addApplicationFont(qsFontFile);
        g_logger << "newFontId: " >> fontId;
        if (-1 != fontId)
        {
            cauto qslst = QFontDatabase::applicationFontFamilies(fontId);
            if (!qslst.empty())
            {
                qsFontfamilyName = qslst.at(0);
                g_logger << "newfamilyName: " >> qsFontfamilyName;
            }
        }

        QFont font(qsFontfamilyName);
        font.setPointSizeF(fFontSize);
        font.setWeight((int)eWeight);
        g_mapFont[eWeight] = font;

        if (__defFontWeight == eWeight)
        {
            this->setFont(font);
        }
    });
}

static void _resetRootDir(wstring& strRootDir)
{
#if __android
    strRootDir = L"/sdcard/XMusic";

#elif __mac
    strRootDir = fsutil::getHomeDir() + L"/XMusic";

#elif __ios
#if TARGET_IPHONE_SIMULATOR
    strRootDir = L"/Users/lhyuan/XMusic";
#else
    strRootDir = fsutil::getHomeDir() + L"/XMusic";
#endif

#else
    strRootDir = fsutil::getHomeDir() + L"/XMusic";
#endif

    (void)fsutil::createDir(strRootDir + __medialibPath);
}

int CXMusicApp::run()
{
    m_mainWnd.showLogo();

    timerutil::async([&](){
        auto& option = m_model.init();
        auto& strRootDir = option.strRootDir;
#if __windows && !__onlineMediaLib
        if (strRootDir.empty() || !fsutil::existDir(strRootDir))
        {
            CFolderDlg FolderDlg;
            strRootDir = FolderDlg.Show((HWND)m_mainWnd.winId(), NULL, L"设定媒体库路径", L"请选择媒体库目录");
            if (strRootDir.empty())
            {
                this->quit();
                return false;
            }
        }
#else
        _resetRootDir(strRootDir);
#endif

        timerutil::async(6000, [&](){
            if(!_run())
            {
                this->quit();
                return;
            }

            m_mainWnd.show();
        });
    });

    int nRet = exec();

    g_logger >> "stop controller";
    m_ctrl.stop();

    CPlayer::QuitSDK();

    g_logger >> "quit";

    m_logger.close();

    return nRet;
}

bool CXMusicApp::_run()
{
#if __onlineMediaLib
    g_logger >> "upgradeMediaLib";
    if (!m_model.upgradeMediaLib())
    {
        g_logger >> "upgradeMediaLib fail";
        return false;
    }
#endif

    g_logger >> "initMediaLib";
    if (!m_model.initMediaLib())
    {
        g_logger >> "initMediaLib fail";
        return false;
    }

    g_logger >> "start controller";
    if (!m_ctrl.start())
    {
        g_logger >> "start controller fail";
        return false;
    }
    g_logger >> "start controller success, app running";

    return true;
}
