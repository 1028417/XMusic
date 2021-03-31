
#include "xmusic.h"

#include "dlg/networkWarnDlg.h"

#include "dlg/msgbox.h"

CApp::CApp()
    : m_ctrl(*this, m_model)
    , m_model(m_mainWnd, m_ctrl.getOption())
{
}

int CApp::_exec()
{
    m_ctrl.initOption();
    cauto option = m_ctrl.getOption();
    g_bFullScreen = option.bFullScreen;
    if (option.crBkg >= 0)
    {
        g_crBkg.setRgb((int)option.crBkg);
    }
    if (option.crFore >= 0)
    {
        g_crFore.setRgb((int)option.crFore);
    }

#if !__android
    m_mainWnd.showBlank();
#endif

    auto nRet = CAppBase::_exec();

    m_ctrl.stop();

    return nRet;
}

#if 0
static wstring _genMedialibDir(cwstr strWorkDir)
{
/*#if __window
    if (strRootDir.empty() || !fsutil::existDir(strRootDir))
    {
        CFolderDlg FolderDlg;
        cauto strDir = FolderDlg.Show(m_mainWnd.hwnd(), NULL, L" 设定媒体库路径", L"请选择媒体库目录");
        if (!strDir.empty())
        {
            return strDir;
        }
    }
#endif

#if __mac
#if __ios && TARGET_IPHONE_SIMULATOR
    //return L"/Users/lhyuan/XMusic";
#endif

    return fsutil::getHomeDir() + __WS2Q(L"/XMusic")).toStdWString();
#endif*/

    return strWorkDir;
}
#endif

bool CApp::_startup(cwstr strWorkDir)
{
    __app.sync([&]{
        m_mainWnd.showLogo();
    });

    //auto timeBegin = time(0);
    auto strMedialibDir = strWorkDir; //_genMedialibDir(strWorkDir);
    if (!m_model.init(strWorkDir, strMedialibDir))
    {
        g_logger >> "initModel fail";
        return false;
    }

    QFile qf(":/mdlconf");
    if (!qf.open(QFile::OpenModeFlag::ReadOnly))
    {
        g_logger >> "loadMdlConfResource fail";
        return false;
    }
    CByteBuffer bbfConf(qf.size());
    qf.read((char*)bbfConf.ptr(), qf.size());

    /*string strUrl = "https://xmusic-generic.pkg.coding.net/xmusic/user/xmusic";
    CByteBuffer bbfUserProfile;
    CDownloader downloader(3, 30, 1, 3);
    int nRet = downloader.syncDownload(g_bRunSignal, strUrl, bbfUserProfile, 1);
    if (0 != nRet)
    {
        return E_UpgradeResult::UR_Fail;
    }*/

    E_UpgradeResult eUpgradeResult = mtutil::concurrence([&]{
        auto time0 = time(0);
        extern int g_nAppUpgradeProgress;
        E_UpgradeResult eUpgradeResult = m_model.upgradeMdl(bbfConf, g_bRunSignal
                                                            , (UINT&)g_nAppUpgradeProgress, m_strAppVersion);
        if (E_UpgradeResult::UR_Success != eUpgradeResult)
        {
            g_logger << "upgradeMdl fail: " >> (int)eUpgradeResult;

            if (g_bRunSignal && m_model.initMediaLib())
            {
                return E_UpgradeResult::UR_Success;
            }
            return eUpgradeResult;
        }
        g_logger << "upgradeMdl success " >> (time(0)-time0);

        if (!g_bRunSignal)
        {
            return E_UpgradeResult::UR_Fail;
        }

        if (!m_model.initMediaLib())
        {
            g_logger >> "initMediaLib fail";
            return E_UpgradeResult::UR_InitMediaLibFail;
        }
        g_logger << "initMediaLib success " >> (time(0)-time0);

        return E_UpgradeResult::UR_Success;
    }, [&]{
        (void)m_pmForward.load(__png(btnForward));
        (void)m_pmHDDisk.load(__mdlPng(hddisk));
        (void)m_pmSQDisk.load(__mdlPng(sqdisk));

        m_mainWnd.preinit();
    });

    sync([=]{
        _show(eUpgradeResult);
    });

    return true;
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
            else if (E_UpgradeResult::UR_InitMediaLibFail == eUpgradeResult)
            {
                qsErrMsg = "读取媒体库失败";
            }
            else //UR_Fail
            {
                qsErrMsg = "加载媒体库失败";
            }

#if __windows
            setForeground();
#elif __android
            vibrate();
#endif
            static CMsgBox m_msgbox(m_mainWnd);
            m_msgbox.show(qsErrMsg, [&]{
                this->quit();
            });
        }

        return;
    }

#if __windows
    setForeground();
#endif

#if __android
    if (m_ctrl.getOption().bNetworkWarn && checkMobileConnected())
    {
        vibrate();

        static CNetworkWarnDlg dlg(m_mainWnd);
        dlg.show([&]{            
            m_ctrl.start();
            m_mainWnd.show();
        });

        return;
    }
#endif

    m_ctrl.start();
    m_mainWnd.show();
}

void CApp::quit()
{
    m_mainWnd.setVisible(false);

    CAppBase::quit();
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
