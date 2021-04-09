
#include "xmusic.h"

#include "dlg/networkWarnDlg.h"

#include "dlg/msgbox.h"

CApp::CApp()
    : m_ctrl(*this, m_model)
    , m_model(m_mainWnd, m_ctrl.getOption())
{
}

int CApp::exec()
{
    g_bFullScreen = m_ctrl.initOption().bFullScreen;
#if __android
    androidFullScreen();
#else
    m_mainWnd.showBlank();
#endif

    auto nRet = CAppBase::exec();

    m_ctrl.stop();

    return nRet;
}

#if 0
static wstring _genMedialibDir()
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

    return L"";
}
#endif

bool CApp::_startup()
{
    CFont::init(this->font());
    sync([&]{
        this->setFont(CFont());

        m_mainWnd.showLogo();
/*#if __android
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0)) // Qt5.7以上
        async([]{
            hideSplashScreen(); //启动页延时关闭防止闪烁
        });
#endif
#endif*/
    });

    //auto timeBegin = time(0);
    //cauto strMedialibDir = _genMedialibDir();

    QFile qf(":/mdlconf");
    if (!qf.open(QFile::OpenModeFlag::ReadOnly))
    {
        g_logger >> "loadMdlConfResource fail";
        return false;
    }
    CByteBuffer bbfMdlConf(qf.size());
    qf.read((char*)bbfMdlConf.ptr(), qf.size());
    if (!m_model.getMdlMgr().loadMdlConf(bbfMdlConf, m_orgMdlConf))
    {
        g_logger >> "readMdlConfResource fail";
        return false;
    }
    strutil::fromAsc(m_orgMdlConf.strAppVersion, m_strAppVer); //并发线程_preinitBkg依赖这个版本号

    wstring strUser;
    E_UpgradeResult eUpgradeResult = mtutil::concurrence([&]{
        g_logger << "orgMdlConf AppVersion: " << m_orgMdlConf.strAppVersion
                 << " CompatibleCode: " << m_orgMdlConf.uCompatibleCode
                 << " MedialibVersion: " >> m_orgMdlConf.uMdlVersion;

        auto time0 = time(0);
        m_model.init(g_strWorkDir);

        string strVerInfo;
        int nRet = curlutil::initCurl(strVerInfo);
        if (nRet != 0)
        {
            g_logger << "initCurl fail: " >> nRet;
            return E_UpgradeResult::UR_Fail;
        }
        g_logger << "CurlVerInfo: \n" >> strVerInfo;

        string strPwd;
        strUser = m_model.getUserMgr().loadProfile(strPwd);
        if (!strUser.empty())
        {
            //_syncLogin(g_bRunSignal, strUser, strPwd);
            asyncLogin(strUser, strPwd);
        }

        E_UpgradeResult eUpgradeResult = m_model.getMdlMgr().upgradeMdl(g_bRunSignal, m_orgMdlConf);
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
        auto& opt = m_ctrl.getOption();
        if (opt.crBkg >= 0)
        {
            g_crBkg.setRgb((int)opt.crBkg);
        }
        if (opt.crFore >= 0)
        {
            g_crFore.setRgb((int)opt.crFore);
        }

        (void)m_pmForward.load(__png(btnForward));
        (void)m_pmHDDisk.load(__mdlPng(hddisk));
        (void)m_pmSQDisk.load(__mdlPng(sqdisk));

        m_mainWnd.preinit();
    });

    sync([=]{
        _show(eUpgradeResult, strUser);
    });

    return true;
}

#if __windows
static void _setForeground()
{
    auto hwnd = __app.mainWnd().hwnd();
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
#else
#define _setForeground()
#endif

void CApp::_show(E_UpgradeResult eUpgradeResult, cwstr strUser)
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

#if __android
            vibrate();
#endif
            _setForeground();
            static CMsgBox m_msgbox;//(m_mainWnd);
            m_msgbox.show(qsErrMsg, [&]{
                this->quit();
            });
        }

        return;
    }

#if __android
    if (m_ctrl.getOption().bNetworkWarn && checkMobileConnected())
    {
        vibrate();

        static CNetworkWarnDlg dlg;
        dlg.show([&, strUser]{
            _show(strUser);
        });

        return;
    }
#endif

    _setForeground();
    _show(strUser);
}

static void _showLoginDlg(cwstr strUser = L"", const string& strPwd = "", E_LoginReult eRet = E_LoginReult::LR_Success)
{
#if __android
    vibrate();
#else
    _setForeground();
#endif

    static CLoginDlg m_loginDlg;
    m_loginDlg.show(strUser, strPwd, eRet);
}

void CApp::_show(cwstr strUser)
{
    if (strUser.empty())
    {
        __async(3000, [&]{
            _showLoginDlg();
        });
    }

    m_ctrl.start();
    m_mainWnd.show();
}

//static UINT s_uSeq = 0;
void CApp::_cbLogin(E_LoginReult eRet, cwstr strUser, const string& strPwd, bool bRelogin)
{
    //if (uSeq != s_uSeq) return;
    if (E_LoginReult::LR_Success == eRet)
    {
        sync(6e5, [=]{
            //if (uSeq != s_uSeq) return;
            (void)asyncLogin(strUser, strPwd, true);
        });
#if __android
        if (!bRelogin)
        {
            vibrate();
            showToast("登录成功！Hi，" + __WS2Q(strUser), true);
        }
#endif
    }
    else
    {
        sync(3000, [=]{
            //if (uSeq != s_uSeq) return;
            _showLoginDlg(strUser, strPwd, eRet);
        });
    }
}

E_LoginReult CApp::_syncLogin(signal_t bRunSignal, cwstr strUser, const string& strPwd, bool bRelogin)
{
    //auto uSeq = ++s_uSeq;

    auto eRet = m_model.getUserMgr().syncLogin(bRunSignal, strUser, strPwd);
    _cbLogin(eRet, strUser, strPwd, bRelogin);
    return eRet;
}

void CApp::asyncLogin(cwstr strUser, const string& strPwd, bool bRelogin)
{
    //auto uSeq = ++s_uSeq;

    m_model.getUserMgr().asyncLogin(g_bRunSignal, strUser, strPwd, [=](E_LoginReult eRet){
        _cbLogin(eRet, strUser, strPwd, bRelogin);
    });
    return;

    static auto& thr = this->thread();
    if (thr)
    {
        mtutil::thread([=]{
            thr.cancel();
            thr.start([=]{
                (void)_syncLogin(thr, strUser, strPwd, bRelogin);
            });
        });
    }
    else
    {
        thr.join();
        thr.start([=]{
            (void)_syncLogin(thr, strUser, strPwd, bRelogin);
        });
    }
}

void CApp::quit()
{
    m_mainWnd.setVisible(false);

    CAppBase::quit();
}
