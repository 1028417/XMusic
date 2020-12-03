#pragma once

#include "xmusic.h"

#include <QApplication>

extern ITxtWriter& g_logger;

struct tagScreenInfo
{
    int szScreenMax = 0;
    int szScreenMin = 0;
    float fPixelRatio = 1;
    float fDPI = 0;
};
extern const tagScreenInfo& g_screen;

extern const bool& g_bRunSignal;

#include "mainwindow.h"

//#include "dlg/msgbox.h"

const bool& usleepex(UINT uMs);

class CAppInit : public QApplication
{
protected:
    CAppInit();

    void setupFont();
};

Q_DECLARE_METATYPE(fn_void);

template <typename T>
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
};

#define __app CApp::inst()

class CApp : public CAppInit, private IPlayerView, public tagSingleTone<CApp>
{
    Q_OBJECT
private:
    CApp();
    friend tagSingleTone;

public:
    QPixmap m_pmForward; // 列表末端箭头
    QPixmap m_pmHDDisk;
    QPixmap m_pmLLDisk;

private:
    CXController m_ctrl;

    CModel m_model;

    wstring m_strAppVersion;

    list<XThread> m_lstThread;

    MainWindow m_mainWnd;

    //CMsgBox m_msgbox;

signals:
    void signal_sync(fn_void cb);

private:
    IModelObserver& getModelObserver() override
    {
        return m_mainWnd;
    }

    E_UpgradeResult _init();

    void _startup();
    void _show(E_UpgradeResult eUpgradeResult);

public:
    XThread& thread()
    {
        m_lstThread.emplace_back();
        return m_lstThread.back();
    }

    template<typename... T>
    XThread& thread(const T&... args)
    {
        m_lstThread.emplace_back();
        auto& thr = m_lstThread.back();
        thr.start(args...);
        return thr;
    }

    void sync(cfn_void cb);
    void sync(UINT uDelayTime, cfn_void cb);

    tagOption& getOption()
    {
        return m_ctrl.getOption();
    }

    IXController& getCtrl()
    {
        return m_ctrl;
    }

    MainWindow& mainWnd()
    {
        return m_mainWnd;
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

    int run(cwstr strWorkDir);

    void quit();

#if __windows
    void setForeground();
#endif
};
