
#include "app.h"

#include "mainwindow.h"

#include <QFontDatabase>

#include <QScreen>

float g_fPixelRatio = 1;

static CUTF8TxtWriter m_logger;
ITxtWriter& g_logger(m_logger);

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

CAppInit::CAppInit(QApplication& app)
{
#if __windows
    fsutil::setWorkDir(fsutil::getModuleDir());
#elif __android
    fsutil::setWorkDir(__androidDataDir);
#else
    fsutil::setWorkDir(app.applicationDirPath().toStdWString());
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
    fFontSize = app.font().pointSizeF();
    fFontSize = fFontSize*nScreenWidth/540;

#elif __mac
    fFontSize = 29;

#elif __windows
    fFontSize = 22;

    float fDPIRate = getDPIRate();
    g_logger << "DPIRate: " >> fDPIRate;
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
            app.setFont(font);
        }
    });
}

bool CApp::m_bRunSignal = true;

void CApp::async(UINT uDelayTime, cfn_void cb)
{
    __async(uDelayTime, [&, cb](){
        if (!m_bRunSignal)
        {
            return;
        }

        cb();
    });
}


bool CApp::_resetRootDir(wstring& strRootDir)
{
    strRootDir = fsutil::getHomeDir() + L"/XMusic";

#if __android
    strRootDir = L"/sdcard/XMusic";

//#elif __ios && TARGET_IPHONE_SIMULATOR
//    strRootDir = L"/Users/lhyuan/XMusic";

#elif __window
    if (!XMediaLib::m_bOnlineMediaLib)
    {
        if (strRootDir.empty() || !fsutil::existDir(strRootDir))
        {
            CFolderDlg FolderDlg;
            strRootDir = FolderDlg.Show(m_mainWnd.hwnd(), NULL, L" 设定媒体库路径", L"请选择媒体库目录");
            if (strRootDir.empty())
            {
                return false;
            }
        }

        return true;
    }
#endif

    return fsutil::createDir(strRootDir + __medialibPath);
}

void CApp::slot_run(bool bUpgradeResult)
{
    if (!m_bRunSignal)
    {
        return;
    }

    if (!bUpgradeResult)
    {
        m_msgbox.show("更新媒体库失败", [&](){
            this->quit();
        });
        return;
    }

    if (!m_model.initMediaLib())
    {
        g_logger >> "initMediaLib fail";
        this->quit();
        return;
    }

    if (!m_ctrl.start())
    {
        g_logger >> "start controller fail";
        this->quit();
        return;
    }
    g_logger >> "start controller success, app running";

    if (m_ctrl.getOption().crTheme >= 0)
    {
        g_crTheme.setRgb((int)m_ctrl.getOption().crTheme);
    }
    if (m_ctrl.getOption().crText >= 0)
    {
        g_crText.setRgb((int)m_ctrl.getOption().crText);
    }

    m_mainWnd.show();
}

int CApp::run()
{
    auto& option = m_ctrl.initOption();

    std::thread thrUpgrade;

    CApp::async(100, [&](){
        if (!_resetRootDir(option.strRootDir))
        {
            this->quit();
            return;
        }
        g_logger << "RootDir: " >> option.strRootDir;

        connect(this, &CApp::signal_run, this, &CApp::slot_run);

        thrUpgrade = std::thread([&]() {
            auto timeBegin = time(NULL);

            bool bUpgradeResult = true;
            if (XMediaLib::m_bOnlineMediaLib)
            {
                bUpgradeResult = _upgradeMediaLib();
            }

            auto timeWait = 6 - (time(NULL) - timeBegin);
            if (timeWait > 0)
            {
                mtutil::usleep((UINT)timeWait*1000);
            }

            emit signal_run(bUpgradeResult);
        });
    });

    m_mainWnd.showLogo();

    int nRet = exec();
    m_bRunSignal = false;

    if (thrUpgrade.joinable())
    {
        thrUpgrade.join();
    }

    g_logger >> "stop controller";
    m_ctrl.stop();

    CPlayer::QuitSDK();

    g_logger >> "quit";
    m_logger.close();

    return nRet;
}

bool CApp::_upgradeMediaLib()
{
    QFile qf(":/medialib.conf");
    if (!qf.open(QFile::OpenModeFlag::ReadOnly))
    {
        g_logger >> "loadMedialibConfResource fail";
        return false;
    }

    cauto ba = qf.readAll();
    IFBuffer ifbMedialibConf((c_byte_p)ba.data(), ba.size());
    tagMedialibConf orgMedialibConf;
    __EnsureReturn(m_model.readMedialibConf(orgMedialibConf, &ifbMedialibConf), false);
    g_logger << "MediaLib orgVersion: " >> orgMedialibConf.uMedialibVersion;

    tagMedialibConf *pPrevMedialibConf = &orgMedialibConf;
    tagMedialibConf userMedialibConf;
    if (m_model.readMedialibConf(userMedialibConf))
    {
        if (userMedialibConf.uCompatibleAppVersion == orgMedialibConf.uCompatibleAppVersion)
        {
            g_logger << "MediaLib userVersion: " >> userMedialibConf.uMedialibVersion;

            if (userMedialibConf.uMedialibVersion >= orgMedialibConf.uMedialibVersion)
            {
                pPrevMedialibConf = &userMedialibConf;
            }
        }
    }

    if (!m_model.upgradeMediaLib(*pPrevMedialibConf, [&](int64_t dltotal, int64_t dlnow){
        (void)dltotal;
        (void)dlnow;
        return m_bRunSignal;
    }))
    {
        g_logger >> "upgradeMediaLib fail";
        return false;
    }

    return true;
}
