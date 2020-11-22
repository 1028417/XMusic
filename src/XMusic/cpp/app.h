#pragma once

#include "viewdef.h"
#include "model/model.h"
#include "../controller.h"

#include <QApplication>

#include "mainwindow.h"

#include "msgbox.h"

#include "androidutil.h"

#if __windows || __mac
#include <QLockFile>
extern QLockFile g_lf;
#endif

#define __pkgName L"com.musicrossoft.xmusic"

#define __cyIPhoneXBangs __size(128)

#define __mediaPng(f) ":/img/medialib/" #f ".png"

extern const WString& mediaQualityString(E_MediaQuality eQuality);

extern ITxtWriter& g_logger;

extern int g_szScreenMax;
extern int g_szScreenMin;

extern int g_argc;
extern char **g_argv;

class CAppInit : public QApplication
{
protected:
    CAppInit() : QApplication(g_argc, g_argv)
    {
    }
};

Q_DECLARE_METATYPE(fn_void);

template <typename T>
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

template <typename T>
struct tagSingleTone
{
    static T& inst()
    {
        static T *inst = NULL;
        if (NULL == inst)
        {
            inst = (T*)malloc(sizeof(T));
            new (inst) T();
        }

        static tagSinglePtr<T> SinglePtr(inst);

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
    QPixmap m_pmHDDisk;
    QPixmap m_pmLLDisk;

private:
    CXController m_ctrl;

    CModel m_model;

    wstring m_strAppVersion;

    list<XThread> m_lstThread;

    MainWindow m_mainWnd;

    CMsgBox m_msgbox;

signals:
    void signal_run(int nUpgradeResult);

    void signal_sync(fn_void cb);

private:
    IModelObserver& getModelObserver() override
    {
        return m_mainWnd;
    }

    bool _init(cwstr strWorkDir);

    void _run(E_UpgradeResult eUpgradeResult);

public:
    static bool checkIPhoneXBangs(int cx, int cy)
    {
        return __ios && ((375 == cx && 812 == cy) || (414 == cx && 896 == cy));
    }

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

    static void async(cfn_void cb)
    {
        async(0, cb);
    }

    static void async(UINT uDelayTime, cfn_void cb);

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
