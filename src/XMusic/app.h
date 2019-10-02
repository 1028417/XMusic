#pragma once

#include "viewdef.h"
#include "model/model.h"
#include "../XMusicHost/controller.h"

#include <QApplication>

#include "mainwindow.h"

#include <QTimer>

extern ITxtWriter& g_logger;

class CApp : public QApplication,  public IPlayerView
{
public:
    CApp(int argc, char **argv);

private:
    MainWindow m_mainWnd;

    CModel m_model;

    CController m_ctrl;

private:
    IModelObserver& getModelObserver()
    {
        return m_mainWnd;
    }

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

    IPlayerController& getCtrl()
    {
        return m_ctrl;
    }

    int run();

    void setTimer(UINT uMs, const function<bool()>& cb)
    {
        QTimer::singleShot(uMs, [=](){
            if (cb())
            {
                setTimer(uMs, cb);
            }
        });
    }
};

extern CApp *g_app;
