#pragma once

#include "viewdef.h"
#include "model/model.h"
#include "../controller.h"

#include <QApplication>

#include "mainwindow.h"

#include "msgbox.h"

#define __cyIPhoneXBangs __size(128)

extern ITxtWriter& g_logger;

class CAppInit
{
protected:
    CAppInit(QApplication& app);

protected:
    string m_strOrgDir;
};

enum class E_UpgradeErrMsg
{
    UEM_None = 0,
    UEM_AppUpgradeFail,
    UEM_AppUpgraded
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

signals:
    void signal_run(bool bUpgradeFail, int nUpgradeErrMsg);

    //void signal_appUpgradeProgress();

private slots:
    void slot_run(bool bUpgradeResult, int nUpgradeErrMsg);

    //void slot_appUpgradeProgress();

private:
    IModelObserver& getModelObserver() override
    {
        return m_mainWnd;
    }

    bool _resetRootDir(wstring& strRootDir);

    bool _readMedialibConf(Instream& ins, tagMedialibConf& medialibConf);

    bool _upgradeMediaLib(E_UpgradeErrMsg& eUpgradeErrMsg);
    bool _upgradeMedialib(tagMedialibConf& prevMedialibConf, E_UpgradeErrMsg& eUpgradeErrMsg);

    bool _upgradeApp(const string& strPrevVersion, const tagMedialibConf& newMedialibConf);

public:
    const string& orgDir() const
    {
        return m_strOrgDir;
    }

    static bool checkIPhoneXBangs(int cx, int cy)
    {
        return __ios && ((375 == cx && 812 == cy) || (414 == cx && 896 == cy));
    }

    static void async(UINT uDelayTime, cfn_void cb);
    static void async(cfn_void cb)
    {
        async(0, cb);
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

    int run();

    void quit()
    {
        m_bRunSignal = false;
        m_mainWnd.close();
        QApplication::quit();
    }
};
