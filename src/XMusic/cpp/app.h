#pragma once

#include "viewdef.h"
#include "model/model.h"
#include "../controller.h"

#include <QApplication>

#include "mainwindow.h"

#include "msgbox.h"

#define __androidDataDir L"/sdcard/Android/data/com.musicrossoft.xmusic"

extern ITxtWriter& g_logger;

class CApp : public QApplication
{
public:
    CApp(int argc, char **argv);
};

class CXMusicApp : public CApp,  public IPlayerView
{
    Q_OBJECT
public:
    CXMusicApp(int argc, char **argv) :
        CApp(argc, argv),
        m_ctrl(*this, m_model),
        m_model(m_mainWnd, m_ctrl.getOption()),
        m_mainWnd(*this)
    {
    }

private:
    CXController m_ctrl;

    CModel m_model;

    MainWindow m_mainWnd;

    bool m_bRunSignal = false;

signals:
    void signal_run(bool bUpgradeResult);

private slots:
    void slot_run(bool bUpgradeResult);

private:
    IModelObserver& getModelObserver() override
    {
        return m_mainWnd;
    }

    bool _resetRootDir(wstring& strRootDir);

    bool _upgradeMediaLib();

public:
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
};
