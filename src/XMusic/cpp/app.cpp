
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
    cauto fnStartup = [&]{
        m_mainWnd.startLogo();

        (void)this->thread([=](XThread& thr){
           (void)_startup(thr);
        });
    };

    g_bFullScreen = m_ctrl.initOption().bFullScreen;
#if __android
    androidFullScreen();

    init();
    CFont::init(this->font());
    this->setFont(CFont());
    m_mainWnd.showLogo();

    if (m_ctrl.getOption().bNetworkWarn && checkMobileConnected())
    {
        async([=]{
            vibrate();

            CNetworkWarnDlg::inst().show([=]{
                fnStartup();
            });
        });
    }
    else
    {
        fnStartup();
    }

#else
    m_mainWnd.showBlank();
    async([=]{
        init();
        CFont::init(this->font());
        this->setFont(CFont());
        m_mainWnd.showLogo();

        fnStartup();
    });
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

bool CApp::_startup(XThread& thr)
{
    auto time0 = time(0);

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

        m_model.init(g_strWorkDir);

        string strVerInfo;
        int nRet = curlutil::initCurl(strVerInfo);
        if (nRet != 0)
        {
            g_logger << "initCurl fail: " >> nRet;
            return E_UpgradeResult::UR_Fail;
        }
        g_logger << "CurlVerInfo: \n" >> strVerInfo;

        signal_t bRunSignal = thr;
        string strPwd;
        strUser = m_model.getUserMgr().loadProfile(strPwd);
        if (!strUser.empty())
        {
            auto eRet = m_model.getUserMgr().syncLogin(bRunSignal, strUser, strPwd);
            if (E_LoginReult::LR_NetworkError == eRet)
            {
                return E_UpgradeResult::UR_NetworkError;
            }
            _cbLogin(eRet, strUser, strPwd, false);
            //_syncLogin(bRunSignal, strUser, strPwd); //asyncLogin(strUser, strPwd);//并行跑安卓大概率闪退
        }

        E_UpgradeResult eUpgradeResult = m_model.getMdlMgr().upgradeMdl(bRunSignal, m_orgMdlConf);
        if (E_UpgradeResult::UR_Success != eUpgradeResult)
        {
            g_logger << "upgradeMdl fail: " >> (int)eUpgradeResult;

            if (bRunSignal && m_model.initMediaLib())
            {
                return E_UpgradeResult::UR_Success;
            }
            return eUpgradeResult;
        }
        g_logger << "upgradeMdl success " >> (time(0)-time0);

        if (!bRunSignal)
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

        m_mainWnd.preinit(thr);

        g_logger << "preinit success " >> (time(0)-time0);
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
            if (E_UpgradeResult::UR_NetworkError == eUpgradeResult)
            {
                qsErrMsg = "更新媒体库失败:  网络异常";
            }
            else if (E_UpgradeResult::UR_ResponseError == eUpgradeResult)
            {
                qsErrMsg = "更新媒体库失败:  服务器异常";
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

            m_mainWnd.stopLogo();
            static CMsgBox m_msgbox;//(m_mainWnd);
            m_msgbox.show(qsErrMsg, [&]{
                async([&]{
                    this->quit();
                });
            });
        }

        return;
    }

    if (strUser.empty())
    {
        async(2000, [&]{
            _showLoginDlg();
        });
    }

    m_ctrl.start();

    _setForeground();
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
                auto eRet = m_model.getUserMgr().syncLogin(thr, strUser, strPwd);
                _cbLogin(eRet, strUser, strPwd, bRelogin);
            });
        });
    }
    else
    {
        thr.join();
        thr.start([=]{
            auto eRet = m_model.getUserMgr().syncLogin(thr, strUser, strPwd);
            _cbLogin(eRet, strUser, strPwd, bRelogin);
        });
    }
}

void CApp::quit()
{
#if __android
    this->_quit();
#else
    m_mainWnd.quit([&]{
        async([&]{
            this->_quit();
        });
    });
#endif
}
