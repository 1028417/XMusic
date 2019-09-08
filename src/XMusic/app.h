#pragma once

#include "viewdef.h"
#include "model/model.h"
#include "../XMusicHost/controller.h"

#include <QApplication>
#include <QMainWindow>

#include <QTimer>

extern ITxtWriter& g_logger;

class CPlayerView : public IPlayerView
{
public:
    CPlayerView(QApplication& app, class MainWindow& mainWnd, IModel& model, IPlayerController& ctrl) :
        m_app(app),
        m_mainWnd(mainWnd),
        m_model(model),
        m_ctrl(ctrl)
    {
    }

private:
    QApplication& m_app;

    class MainWindow& m_mainWnd;

    IModel& m_model;

    IPlayerController& m_ctrl;

public:
    QApplication& getApp() const
    {
        return m_app;
    }

    QMainWindow& getMainWnd() const;

    IPlayerController& getCtrl() const
    {
        return m_ctrl;
    }

    IModel& getModel() const
    {
        return m_model;
    }

    COptionMgr& getOptionMgr() const
    {
        return m_model.getOptionMgr();
    }

    CDataMgr& getDataMgr() const
    {
        return m_model.getDataMgr();
    }

    CPlayMgr& getPlayMgr() const
    {
        return m_model.getPlayMgr();
    }

    void setTimer(UINT uMs, const function<bool()>& cb)
    {
        QTimer::singleShot(uMs, [=](){
            if (cb())
            {
                setTimer(uMs, cb);
            }
        });
    }

private:
    class IModelObserver& getModelObserver() override;
};
