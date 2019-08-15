#pragma once

#include "ViewDef.h"
#include "model.h"
#include "../PlayerApp/controller.h"

#include <QFont>

extern ITxtWriter& g_logWriter;

enum class E_FontWeight
{
    FW_Light = QFont::Weight::Light,
    //FW_Normal = QFont::Weight::Normal,
    FW_SemiBold = QFont::Weight::DemiBold
};

#define __defFontWeight E_FontWeight::FW_Light

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

    QFont genFont(double dbOffsetSize, E_FontWeight eWeight = __defFontWeight, bool bItalic=false) const;

    void setFont(QWidget *widget, double dbOffsetSize=0
            , E_FontWeight eWeight = __defFontWeight, bool bItalic=false) const;

    void setTimer(UINT uMs, const function<bool()>& cb);

private:
    class IModelObserver& getModelObserver() override;
};
