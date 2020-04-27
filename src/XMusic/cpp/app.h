#pragma once

#include "viewdef.h"
#include "model/model.h"
#include "../controller.h"

#include <QApplication>

#include "mainwindow.h"

#include "msgbox.h"

#define __cyIPhoneXBangs __size(128)

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
    UR_Success,
    UR_Fail,

    UR_DownloadFail,
    UR_MedialibInvalid,
    UR_ReadMedialibFail,

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

    QPixmap m_pmHDDisk;
    QPixmap m_pmLLDisk;

private:
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

    E_UpgradeResult _initMediaLib(const tagMedialibConf& orgMedialibConf);

    bool _initRootDir(wstring& strRootDir);

    E_UpgradeResult _upgradeMedialib(const tagMedialibConf& orgMedialibConf);
    E_UpgradeResult _loadMdl(CZipFile& zipMdl, bool bUpgradeDB);

    bool _upgradeApp(const list<CUpgradeUrl>& lstUpgradeUrl);

public:
    static bool checkIPhoneXBangs(int cx, int cy)
    {
        return __ios && ((375 == cx && 812 == cy) || (414 == cx && 896 == cy));
    }

    static void async(cfn_void cb)
    {
        async(0, cb);
    }

    static void async(UINT uDelayTime, cfn_void cb);

    static void asyncloop(UINT uDelayTime, UINT uTimes, cfn_void cb);
    static void asyncloop(UINT uDelayTime, cfn_bool cb);

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

    int run();

    void quit();

#if __android
    static void vibrate(UINT duration=100);

#elif __windows
    void setForeground();
#endif
};

extern const WString& mediaQualityString(E_MediaQuality eQuality);

#define __mediaPng(f) ":/img/medialib/" #f ".png"
