#pragma once

#include "mainwindow.h"

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

//#define __app CApp::inst()

class CApp : public CAppBase, private IPlayerView//, public tagSingleTone<CApp>
{
public:
    CApp();
    //friend tagSingleTone;

public:
    QPixmap m_pmForward; // 列表末端箭头
    QPixmap m_pmHDDisk;
    QPixmap m_pmLLDisk;

private:
    CXController m_ctrl;

    CModel m_model;

    wstring m_strAppVersion;

    MainWindow m_mainWnd;

    //CMsgBox m_msgbox;

private:
    IModelObserver& getModelObserver() override
    {
        return m_mainWnd;
    }

    int _exec() override;

    bool _startup(cwstr strWorkDir) override;

    void _show(E_UpgradeResult eUpgradeResult);

public:
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

    wstring appVersion() const
    {
        return m_strAppVersion;
    }

    void quit();

#if __windows
    void setForeground();
#endif
};
