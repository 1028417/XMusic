#pragma once

#include "viewdef.h"
#include "model/model.h"
#include "../controller.h"

#include <QApplication>

#include "mainwindow.h"

#include "msgbox.h"

#define __cyIPhoneXBangs __size(128)

#define __appAsync CApp::async

#if __windows || __mac
#include <QLockFile>
extern QLockFile g_lf;
#endif

extern ITxtWriter& g_logger;

extern int g_nAppDownloadProgress;

class CAppInit
{
protected:
    CAppInit(QApplication& app);
};

enum class E_UpgradeResult
{
    UR_None = 0,
    UR_Success,
    UR_Fail,
    UR_MedialibUncompatible,
    UR_AppUpgradeFail,
    UR_AppUpgraded
};

class CApp : public QApplication, private CAppInit, private IPlayerView
{
    Q_OBJECT
public:
    CApp(int argc, char **argv) :
        QApplication(argc, argv),
        CAppInit((QApplication&)*this),
        m_ctrl(*this, m_model),
        m_model(m_mainWnd, m_ctrl.getOption()),
        m_mainWnd(*this),
        m_msgbox(m_mainWnd)
    {
    }

private:
    static bool m_bRunSignal;

    CXController m_ctrl;

    CModel m_model;

    MainWindow m_mainWnd;

    CMsgBox m_msgbox;

    wstring m_strAppVersion;

signals:
    void signal_run(int nUpgradeResult);

private slots:
    void slot_run(int nUpgradeResult);

private:
    IModelObserver& getModelObserver() override
    {
        return m_mainWnd;
    }

    bool _resetRootDir(wstring& strRootDir);

    bool _readMedialibConf(Instream& ins, tagMedialibConf& medialibConf);

    E_UpgradeResult _upgradeMedialib(const tagMedialibConf& orgMedialibConf);

    bool _upgradeApp(const list<CUpgradeUrl>& lstUpgradeUrl);

public:
    static bool checkIPhoneXBangs(int cx, int cy)
    {
        return __ios && ((375 == cx && 812 == cy) || (414 == cx && 896 == cy));
    }

    static void async(UINT uDelayTime, cfn_void cb);

    static void async(cfn_void cb)
    {
        async(0, cb);
    }

    static void async(UINT uDelayTime, UINT uTimes, cfn_void cb)
    {
        async(uDelayTime, [=](){
            cb();

            if (uTimes > 1)
            {
                async(uDelayTime, uTimes-1, cb);
            }
        });
    }

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

    XMediaLib& getMediaLib()
    {
        return m_model.getMediaLib();
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

    int run();

    void quit();
};
