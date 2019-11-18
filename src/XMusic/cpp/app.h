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
public:
    CXMusicApp(int argc, char **argv) :
        CApp(argc, argv),
        m_model(m_mainWnd),
        m_ctrl(*this, m_model),
        m_mainWnd(*this)
    {
    }

private:
    CModel m_model;

    CXController m_ctrl;

    MainWindow m_mainWnd;

private:
    IModelObserver& getModelObserver() override
    {
        return m_mainWnd;
    }

    bool _resetRootDir(wstring& strRootDir);

    bool _upgradeMediaLib();
    bool _upgradeMediaLib(UINT uVersion, CZipFile& zipFile);

public:
    MainWindow& mainWnd()
    {
        return m_mainWnd;
    }

    IModel& getModel()
    {
        return m_model;
    }

    COptionMgr& getOptionMgr()
    {
        return getModel().getOptionMgr();
    }

    CDataMgr& getDataMgr()
    {
        return getModel().getDataMgr();
    }

    CPlayMgr& getPlayMgr()
    {
        return getModel().getPlayMgr();
    }

    IXController& getCtrl()
    {
        return m_ctrl;
    }

    int run();
};
