
#pragma once

#include "widget.h"

#include "mainwindow.h"

#define __cxBkg 1080
#define __cyBkg 2340

class CCentralWidget : public TWidget<QWidget>
{
public:
    CCentralWidget(QWidget *parent) : TWidget(parent)
    {
    }

private:
    float m_fBkgHWRate = .0f;
    float m_fBkgTopReserve = .0f;

    map<QWidget*, QRect> m_mapTopWidgetPos;
    map<QWidget*, QRect> m_mapWidgetPos;
    map<QWidget*, QRect> m_mapWidgetNewPos;

public:
    void init();

    void relayout(int cx, int cy, bool bDefaultBkg, E_SingerImgPos eSingerImgPos
                  , const tagPlayingInfo& PlayingInfo, CPlayingList& PlayingList);

    float caleBkgZoomRate(int& cxDst, int cyDst, int& xDst);

private:
    void _onTouchEvent(E_TouchEventType type, const CTouchEvent& te) override
    {
        ((MainWindow*)parent())->handleTouchEvent(type, te);
    }
};
