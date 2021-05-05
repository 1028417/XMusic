#pragma once

#include "mainwindow.h"

#include "dlg/logindlg.h"

//#include "dlg/msgbox.h"

/*template <typename T>
struct tagSingleTone
{
    struct tagSinglePtr
    {
        tagSinglePtr(T*ptr) : ptr(ptr)
        {
        }

        ~tagSinglePtr()
        {
            delete ptr;
        }

        T *ptr;
    };

    static T& inst()
    {
        static T *inst = NULL;
        if (NULL == inst)
        {
            inst = (T*)malloc(sizeof(T));
            new (inst) T();
            static tagSinglePtr SinglePtr(inst);
        }

        return *inst;
    }
};*/

//#define g_app CApp::inst()

class CApp : public CAppBase, private IPlayerView//, public tagSingleTone<CApp>
{
public:
    CApp();
    //friend tagSingleTone;

public:
    QPixmap m_pmForward; // 列表末端箭头
    QPixmap m_pmHDDisk;
    QPixmap m_pmSQDisk;

    CLoginDlg m_loginDlg;

private:
    CXController m_ctrl;

    CModel m_model;

    tagMdlConf m_orgMdlConf;
    wstring m_strAppVer;

    MainWindow m_mainWnd;

private:
    IModelObserver& getModelObserver() override
    {
        return m_mainWnd;
    }

    bool _startup(XThread& thr);

    void _show(E_UpgradeResult eUpgradeResult, cwstr strUser, const string& strPwd, E_LoginReult eLoginRet);

    void _showLoginDlg(cwstr strUser, const string& strPwd, E_LoginReult eRet);

    void _cbLogin(E_LoginReult eRet, cwstr strUser, const string& strPwd, bool bRelogin);

public:
    cwstr appVer() const
    {
        return m_strAppVer;
    }

    MainWindow& mainWnd()
    {
        return m_mainWnd;
    }

    IXController& getCtrl()
    {
        return m_ctrl;
    }

    tagOption& getOption()
    {
        return m_ctrl.getOption();
    }

    IModel& getModel()
    {
        return m_model;
    }

    CDataMgr& getDataMgr()
    {
        return m_model.getDataMgr();
    }

    CPlaylistMgr& getPlaylistMgr()
    {
        return m_model.getPlaylistMgr();
    }

    CPlayMgr& getPlayMgr()
    {
        return m_model.getPlayMgr();
    }

    CSingerMgr& getSingerMgr()
    {
        return m_model.getSingerMgr();
    }

    CSingerImgMgr& getSingerImgMgr()
    {
        return m_model.getSingerImgMgr();
    }

    CMdlMgr& getMdlMgr()
    {
        return m_model.getMdlMgr();
    }

    int exec();

    void quit();

    void asyncLogin(cwstr strUser=L"", const string& strPwd="", bool bRelogin=false);
};
