
#pragma once

#include "mainwindow.h"

class CCentralWidget : public CWidget
{
public:
    CCentralWidget(QWidget *parent) : CWidget(parent)
    {
    }

private:
    Double_T m_fBkgHWRate = .0f;
    Double_T m_fBkgTopReserve = .0f;

    map<QWidget*, QRect> m_mapTopWidgetPos;
    map<QWidget*, QRect> m_mapWidgetPos;
    map<QWidget*, QRect> m_mapWidgetNewPos;

public:
    void ctor();

    void relayout(int cx, int cy, bool bDefaultBkg, E_SingerImgPos eSingerImgPos
                  , const tagPlayingInfo& PlayingInfo, CPlayingList& PlayingList);

    Double_T caleBkgZoomRate(Double_T& cxDst, Double_T cyDst, Double_T& xDst);

private:
    void _onTouchEvent(const CTouchEvent& te) override
    {
        ((MainWindow*)parent())->handleTouchEvent(te);
    }
};
