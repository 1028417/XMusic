
#include "xmusic.h"

#include "dlg/networkWarnDlg.h"

#include <QScreen>

tagScreenInfo m_screen;
const tagScreenInfo& g_screen(m_screen);

bool m_bRunSignal = true;
const bool& g_bRunSignal(m_bRunSignal);

static CUTF8TxtWriter m_logger;
ITxtWriter& g_logger(m_logger);

extern int g_argc;
extern char **g_argv;

CAppInit::CAppInit() : QApplication(g_argc, g_argv)
{
    QScreen *screen = QApplication::primaryScreen();
    cauto sz = screen->size();
    m_screen.szScreenMax = sz.width();
    m_screen.szScreenMin = sz.height();
    if (m_screen.szScreenMax < m_screen.szScreenMin)
    {
        std::swap(m_screen.szScreenMax, m_screen.szScreenMin);
    }

    m_screen.fPixelRatio = screen->devicePixelRatio();
    m_screen.fDPI = screen->logicalDotsPerInch();

    m_logger.open("xmusic.log", true);
#if __android
    g_logger << "jniVer: " << g_jniVer << ", androidSdkVer: " >> g_androidSdkVer;
#endif
    g_logger << "screen: " << m_screen.szScreenMax << '*' << m_screen.szScreenMin <<
                ", DPR: " << m_screen.fPixelRatio << ", DPI: " >> m_screen.fDPI;

    g_logger << "applicationDirPath: " >> CApp::applicationDirPath();
    g_logger << "applicationFilePath: " >> CApp::applicationFilePath();

    CFont::init(this->font());

    this->setFont(CFont());
}

CApp::CApp()
    : m_ctrl(*this, m_model)
    , m_model(m_mainWnd, m_ctrl.getOption())
    //, m_msgbox(m_mainWnd)
{
    qRegisterMetaType<fn_void>("fn_void"); //qRegisterMetaType<QVariant>("QVariant");
    connect(this, &CApp::signal_sync, this, [](fn_void cb){
        cb();
    }, Qt::QueuedConnection);
}

#if 0
static wstring _genMedialibDir(cwstr strWorkDir)
{
/*#if __android
    if (requestAndroidPermission("android.permission.WRITE_EXTERNAL_STORAGE"))
    {
        return __sdcardDir __pkgName; //API 23以上需要动态申请读写权限
    }

#elif __window
    if (strRootDir.empty() || !fsutil::existDir(strRootDir))
    {
        CFolderDlg FolderDlg;
        cauto strDir = FolderDlg.Show(m_mainWnd.hwnd(), NULL, L" 设定媒体库路径", L"请选择媒体库目录");
        if (!strDir.empty())
        {
            return strDir;
        }
    }

#elif __mac
#if __ios && TARGET_IPHONE_SIMULATOR
    //return L"/Users/lhyuan/XMusic";
#endif

    return fsutil::getHomeDir() + __WS2Q(L"/XMusic")).toStdWString();
#endif*/

    return strWorkDir;
}
#endif

int CApp::run(cwstr strWorkDir)
{
    std::thread thrStartup([=](){
        auto strMedialibDir = strWorkDir; //_genMedialibDir(strWorkDir);
        if (!m_model.init(strWorkDir, strMedialibDir))
        {
            sync([&](){
                this->quit();
            });
            return;
        }

        _startup();
    });

    m_mainWnd.showBlank();

    auto nRet = exec();
    m_bRunSignal = false;

    for (auto& thr : m_lstThread)
    {
        thr.cancel(false);
    }

    for (auto& thr : m_lstThread)
    {
        thr.join();
    }

#if !__android // TODO 规避5.6.1退出的bug
    thrStartup.join();
#endif

    g_logger >> "stop controller";
    m_ctrl.stop();

    g_logger >> "app quit";
    m_logger.close();

    return nRet;
}

void CApp::_startup()
{
    auto timeBegin = time(0);

    QFile qf(":/mdlconf");
    if (!qf.open(QFile::OpenModeFlag::ReadOnly))
    {
        g_logger >> "loadMdlConfResource fail";
        return;
    }
    cauto ba = qf.readAll();

    E_UpgradeResult eUpgradeResult = mtutil::concurrence([&]() {
        extern int g_nAppUpgradeProgress;
        E_UpgradeResult eUpgradeResult = m_model.upgradeMdl((byte_p)ba.data(), ba.size(), g_bRunSignal
                                                            , (UINT&)g_nAppUpgradeProgress, m_strAppVersion);
        if (E_UpgradeResult::UR_Success != eUpgradeResult)
        {
            return eUpgradeResult;
        }

        //auto time0 = time(0);
        if (!m_model.initMediaLib())
        {
            g_logger >> "initMediaLib fail";
            return E_UpgradeResult::UR_Fail;
        }
        //g_logger << "initMediaLib success " >> (time(0)-time0);

        return E_UpgradeResult::UR_Success;
    }, [&](){
        (void)m_pmForward.load(__png(btnForward));
        (void)m_pmHDDisk.load(__mdlPng(hddisk));
        (void)m_pmLLDisk.load(__mdlPng(lldisk));

        m_mainWnd.preinit();
    });

    auto timeDiff = 6 - (time(0) - timeBegin);
    g_logger << "timeDiff: " >> timeDiff;
    if (timeDiff > 0)
    {
        mtutil::usleep((UINT)timeDiff*1000);
    }

    sync([=](){
        _show(eUpgradeResult);
    });
}

void CApp::_show(E_UpgradeResult eUpgradeResult)
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
            static CMsgBox m_msgbox(m_mainWnd);
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
        m_bRunSignal = false;
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
