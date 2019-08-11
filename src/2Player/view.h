#pragma once

#include "ViewDef.h"
#include "model.h"
#include "../PlayerApp/controller.h"

#include <QFont>

extern const ITxtWriter& g_logWriter;

class CPlayerView : public IPlayerView
{
public:
    CPlayerView(class QApplication& app, class MainWindow& mainWnd, IModel& model, IPlayerController& ctrl) :
        m_app(app),
        m_mainWnd(mainWnd),
        m_model(model),
        m_ctrl(ctrl)
    {
    }

private:
    class QApplication& m_app;
    class MainWindow& m_mainWnd;

    IModel& m_model;

    IPlayerController& m_ctrl;

public:
    inline bool android() const
    {
#ifdef __ANDROID__
        return true;
#else
        return false;
#endif
    }

    QApplication& getApp() const
    {
        return m_app;
    }

    MainWindow& getMainWnd() const
    {
        return m_mainWnd;
    }

    IPlayerController& getCtrl() const
    {
        return m_ctrl;
    }

    IModel& getModel() const
    {
        return m_model;
    }

    CDataMgr& getDataMgr() const
    {
        return m_model.getDataMgr();
    }

    CPlayMgr& getPlayMgr() const
    {
        return m_model.getPlayMgr();
    }

    //QFont font() const;
    QFont genFont(double dbOffsetSize, bool bBold=false, bool bItalic=false) const;

    void setFont(QWidget *widget, double dbOffsetSize=0, bool bBold=false, bool bItalic=false) const;

    void setTextColor(QWidget *widget, const QColor& crText);

    void setTimer(UINT uMs, const function<bool()>& cb);

private:
    class IModelObserver& getModelObserver() override;
};
