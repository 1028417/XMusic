
#include <QFontDatabase>

#include "mainwindow.h"

#include "app.h"

#include <QScreen>

#if __android
//#include <QAndroidJniObject>
//#include <QAndroidJniEnvironment>
//#include <QtAndroid>
//#include <QtAndroidExtras>

#include <jni.h>

int g_jniVer = 0;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    (void)reserved;

    JNIEnv* env = nullptr;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) == JNI_OK)
    {
        return g_jniVer=JNI_VERSION_1_6;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) == JNI_OK)
    {
        return g_jniVer=JNI_VERSION_1_4;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_2) == JNI_OK)
    {
        return g_jniVer=JNI_VERSION_1_2;
    }
    else if (vm->GetEnv((void**) &env, JNI_VERSION_1_1) == JNI_OK)
    {
        return g_jniVer=JNI_VERSION_1_1;
    }

    return JNI_ERR;
}
#endif

static CUTF8Writer m_logger;
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
        return g_strSdcardPath = wsutil::fromStr(pszSdcardPath);
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
    QScreen *screen = QApplication::primaryScreen();
    float fPixelRatio = screen->devicePixelRatio();
    g_logger << "devicePixelRatio: " >> fPixelRatio;
#if __ios
    g_fPixelRatio = (UINT)fPixelRatio;
#endif

    float fFontSize = 0;
#if __android
    fFontSize = 12;
#elif __ios

    QSize szScreen = screen->size();
    int nScreenWidth = MIN(szScreen.width(), szScreen.height()) ;
    fFontSize = this->font().pointSizeF();
    fFontSize = fFontSize*nScreenWidth/540;

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
        qsFontFile = wsutil::toQStr(fsutil::workDir()) + qsFontFile;
#endif

        QString qsFontfamilyName;
        int fontId = QFontDatabase::addApplicationFont(qsFontFile);
        g_logger << "newFontId: " >> fontId;
        if (-1 != fontId)
        {
            cauto& qslst = QFontDatabase::applicationFontFamilies(fontId);
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

int CXMusicApp::run()
{
#if __android
    g_logger << "jniVer: " >> g_jniVer;
#endif

    m_model.init();
    m_mainWnd.showLogo();

    QTimer::singleShot(6000, [&](){
#if __windows
        auto& strRootDir = m_OptionMgr.getOption().strRootDir;

        if (strRootDir.empty())
        {
            g_logger >> "setupRootDir";

            CFolderDlg FolderDlg;
            strRootDir = FolderDlg.Show(hWndParent, NULL, L"设定媒体库路径", L"请选择媒体库目录");
            if (strRootDir.empty())
            {
                g_logger >> "setupRootDir fail";
                this->quit();
                return;
            }
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
            this->quit();
            return;
        }
        g_logger >> "start controller success, app running";

        m_mainWnd.show();
    });

    int nRet = exec();

    g_logger >> "stop controller";
    m_ctrl.stop();

    CPlayer::QuitSDK();

    g_logger >> "quit";
    m_logger.close();

    return nRet;
}

int main(int argc, char *argv[])
{
    //fsutil::zUncompressZip("d:/zlib-1.2.11.zip", "d:/zlib-1.2.11-----");

//#if __windows && (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif

#if __android
    wstring strDataDir = L"/sdcard/Android/data/com.musicrosoft.xmusic";
    if (!fsutil::createDir(strDataDir))
    {
        return -1;
    }
    fsutil::setWorkDir(strDataDir);

#else
    fsutil::setWorkDir(fsutil::getAppDir());
#endif

    m_logger.open(L"XMusic.log", true);

    CXMusicApp app(argc, argv);
    return app.run();
}
