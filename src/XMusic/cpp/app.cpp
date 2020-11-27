
#include "app.h"

#include "mainwindow.h"

#include "networkWarnDlg.h"

#include <QFontDatabase>

#include <QScreen>

static CUTF8TxtWriter m_logger;
ITxtWriter& g_logger(m_logger);

int g_szScreenMax = 0;
int g_szScreenMin = 0;
float g_fPixelRatio = 1;
float g_fDPI = 0;

bool g_bRunSignal = true;

int g_nAppUpgradeProgress = -1;

static const WString g_lpQuality[] {
    L"", L"LQ", L"HQ", L"SQ", L"CD", L"HiRes"
};

const WString& mediaQualityString(E_MediaQuality eQuality)
{
    return g_lpQuality[(UINT)eQuality];
}

extern int g_argc;
extern char **g_argv;

CAppInit::CAppInit() : QApplication(g_argc, g_argv)
{
    QScreen *screen = QApplication::primaryScreen();
    cauto sz = screen->size();
    g_szScreenMax = sz.width();
    g_szScreenMin = sz.height();
    if (g_szScreenMax < g_szScreenMin)
    {
        std::swap(g_szScreenMax, g_szScreenMin);
    }

    g_fPixelRatio = screen->devicePixelRatio();
    g_fDPI = screen->logicalDotsPerInch();

    m_logger.open("xmusic.log", true);
#if __android
    g_logger << "jniVer: " << g_jniVer << ", androidSdkVer: " >> g_androidSdkVer;
#endif
    g_logger << "screen: " << g_szScreenMax << '*' << g_szScreenMin <<
                ", DPR: " << g_fPixelRatio << ", DPI: " >> g_fDPI;

    g_logger << "applicationDirPath: " >> CApp::applicationDirPath();
    g_logger << "applicationFilePath: " >> CApp::applicationFilePath();

    setupFont();
}

static void _setupFont()
{
#if __windows
    g_mapFontFamily[QFont::Weight::Light] = "微软雅黑 Light";
    g_mapFontFamily[QFont::Weight::DemiBold] = "微软雅黑";
    return;
#endif

    list<pair<int, QString>> plFontFile {
        {QFont::Weight::Light, "msyhl-6.23.ttc"}
        , {QFont::Weight::DemiBold, "Microsoft-YaHei-Regular-11.0.ttc"}
    };
    for (auto& pr : plFontFile)
    {
        auto qsFontFile = "/font/" + pr.second;
#if __android
        qsFontFile = "assets:" +  qsFontFile;
#else
        qsFontFile = CApp::applicationDirPath() + qsFontFile;
#endif

        int fontId = QFontDatabase::addApplicationFont(qsFontFile);
        if (-1 == fontId)
        {
            g_logger << "addFont fail: " >> qsFontFile;
            continue;
        }

        cauto qslst = QFontDatabase::applicationFontFamilies(fontId);
        if (!qslst.empty())
        {
            g_logger << "addFont success: " << qsFontFile << " familyName: " >> qslst.front();
            g_mapFontFamily[pr.first] = qslst.front();
        }
    }
}

void CAppInit::setupFont()
{
    cauto font = this->font();

#if __ios
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

    g_uDefFontSize = font.pointSize();
    //g_uDefFontSize *= g_szScreenMax/540.0f;

#elif __mac
    g_uDefFontSize = 28;

#elif __windows
    g_uDefFontSize = 22;

    float fDPIRate = getDPIRate();
    g_logger << "DPIRate: " >> fDPIRate;
    g_uDefFontSize *= fDPIRate;

#elif __android
    g_uDefFontSize = 12;
#endif

    g_nDefFontWeight = QFont::Weight::Light;
    g_mapFontFamily[QFont::Weight::Light] = g_mapFontFamily[QFont::Weight::DemiBold] = font.family();

    _setupFont();

    this->setFont(CFont());
}

CApp::CApp()
    : m_ctrl(*this, m_model),
    m_model(m_mainWnd, m_ctrl.getOption()),
    m_msgbox(m_mainWnd)
{
    qRegisterMetaType<fn_void>("fn_void"); //qRegisterMetaType<QVariant>("QVariant");
    connect(this, &CApp::signal_sync, this, [&](fn_void cb){
        cb();
    }, Qt::QueuedConnection);
}

static void _genRootDir(wstring& strRootDir)
{
#if __android
    if (requestAndroidPermission("android.permission.WRITE_EXTERNAL_STORAGE"))
    {
        //strRootDir = __sdcardDir __pkgName; //API 23以上需要动态申请读写权限
    }

/*#elif __window
    if (strRootDir.empty() || !fsutil::existDir(strRootDir))
    {
        CFolderDlg FolderDlg;
        cauto strDir = FolderDlg.Show(m_mainWnd.hwnd(), NULL, L" 设定媒体库路径", L"请选择媒体库目录");
        if (!strDir.empty())
        {
            strRootDir = strDir;
        }
    }

#elif __mac
    strRootDir = fsutil::getHomeDir() + __WS2Q(L"/XMusic")).toStdWString();

    //#if __ios && TARGET_IPHONE_SIMULATOR
    //    strRootDir = L"/Users/lhyuan/XMusic";
    //#endif*/
#endif
}

int CApp::run(cwstr strWorkDir)
{
    m_mainWnd.showBlank();

    std::thread thrInit([=](){
        auto& strRootDir = m_ctrl.getOption().strRootDir = strWorkDir;
        _genRootDir(strRootDir);
        g_logger << "RootDir: " >> strRootDir;
        if (!fsutil::createDir(m_model.medialibPath()))
        {
            sync([&](){
                this->quit();
            });
            return;
        }

        sync(100, [&](){
            m_mainWnd.showLogo();
        });

        E_UpgradeResult eUpgradeResult = _init();
        sync([=](){
            _run(eUpgradeResult);
        });
    });

    auto nRet = exec();
    g_bRunSignal = false;

    for (auto& thr : m_lstThread)
    {
        thr.cancel(false);
    }

    for (auto& thr : m_lstThread)
    {
        thr.join();
    }

#if !__android // TODO 规避5.6.1退出的bug
    thrInit.join();
#endif

    g_logger >> "stop controller";
    m_ctrl.stop();

    g_logger >> "app quit";
    m_logger.close();

    return nRet;
}

E_UpgradeResult CApp::_init()
{
    return mtutil::concurrence([&](){
        auto timeBegin = time(0);

        QFile qf(":/mdlconf");
        if (!qf.open(QFile::OpenModeFlag::ReadOnly))
        {
            g_logger >> "loadMdlConfResource fail";
            return E_UpgradeResult::UR_Fail;
        }
        auto&& ba = qf.readAll();

        E_UpgradeResult eUpgradeResult = m_model.upgradeMdl((byte_p)ba.data(), ba.size(), g_bRunSignal
                                                            , (UINT&)g_nAppUpgradeProgress, m_strAppVersion);
        if (E_UpgradeResult::UR_Success != eUpgradeResult)
        {
            return eUpgradeResult;
        }

        auto time0 = time(0);
        if (!m_model.initMediaLib())
        {
            g_logger >> "initMediaLib fail";
            return E_UpgradeResult::UR_Fail;
        }
        g_logger << "initMediaLib success " >> (time(0)-time0);

        auto timeDiff = 6 - (time(0) - timeBegin);
        g_logger << "timeDiff: " >> timeDiff;
        if (timeDiff > 0)
        {
            mtutil::usleep((UINT)timeDiff*1000);
        }

        return E_UpgradeResult::UR_Success;
    }, [&](){
        (void)m_pmHDDisk.load(__mediaPng(hddisk));
        (void)m_pmLLDisk.load(__mediaPng(lldisk));

        m_mainWnd.preinit();
    });
}

void CApp::_run(E_UpgradeResult eUpgradeResult)
{
    if (E_UpgradeResult::UR_Success != eUpgradeResult)
    {
        if (E_UpgradeResult::UR_AppUpgraded == eUpgradeResult)
        {
            this->quit();
        }
        else
        {
            QString qsErrMsg;
            if (E_UpgradeResult::UR_DownloadFail == eUpgradeResult)
            {
                qsErrMsg = "更新媒体库失败:  网络异常";
            }
            else if (E_UpgradeResult::UR_MedialibInvalid == eUpgradeResult)
            {
                qsErrMsg = "更新媒体库失败:  媒体库异常";
            }
            else if (E_UpgradeResult::UR_MedialibUnmatch == eUpgradeResult)
            {
                qsErrMsg = "更新媒体库失败:  媒体库不匹配";
            }
            else if (E_UpgradeResult::UR_MedialibUncompatible == eUpgradeResult)
            {
                qsErrMsg = "更新媒体库失败:  媒体库不兼容";
//#if __ios
//#endif
            }
            else if (E_UpgradeResult::UR_AppUpgradeFail == eUpgradeResult)
            {
                qsErrMsg = "更新App失败";
            }
            else
            {
                qsErrMsg = "更新媒体库失败";
            }

#if __windows
            setForeground();
#elif __android
            vibrate();
#endif
            m_msgbox.show(qsErrMsg, [&](){
                this->quit();
            });
        }

        return;
    }

#if __windows
    setForeground();
#endif

    cauto option = m_ctrl.getOption();
    if (option.crBkg >= 0)
    {
        g_crBkg.setRgb((int)option.crBkg);
    }
    if (option.crFore >= 0)
    {
        g_crFore.setRgb((int)option.crFore);
    }

#if __android
    if (option.bNetworkWarn && checkMobileConnected())
    {
        vibrate();

        static CNetworkWarnDlg dlg(m_mainWnd);
        dlg.show([&](){
            m_mainWnd.show();

            m_ctrl.start();
        });

        return;
    }
#endif

    m_mainWnd.show();

    m_ctrl.start();
}

void CApp::quit()
{
    m_mainWnd.setVisible(false);

    sync([&](){
        g_bRunSignal = false;
        QApplication::quit();
    });
}

#if __windows
void CApp::setForeground()
{
    auto hwnd = m_mainWnd.hwnd();
    if (IsIconic(hwnd))
    {
        ::ShowWindow(hwnd, SW_RESTORE);
    }
    else
    {
        //::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        //::SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
        ::SetForegroundWindow(hwnd);
    }
}
#endif

void CApp::sync(cfn_void cb)
{
    if (g_bRunSignal)
    {
        //QVariant var;
        //var.setValue(cb);
        emit signal_sync(cb);
    }
}

inline void async(UINT uDelayTime, cfn_void cb)
{
    if (g_bRunSignal)
    {
        __async(uDelayTime, [&, cb](){
            if (g_bRunSignal)
            {
                cb();
            }
        });
    }
}

void async(cfn_void cb)
{
    async(0, cb);
}
