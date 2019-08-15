
#include <QApplication>

#include "mainwindow.h"

#include "view.h"

#include <QTimer>

IModelObserver& CPlayerView::getModelObserver()
{
    return m_mainWnd;
}

extern map<E_FontWeight, QFont> g_mapFont;

QFont CPlayerView::genFont(float fSizeOffset, E_FontWeight eWeight, bool bItalic) const
{
    QFont font = g_mapFont[eWeight];
    font.setPointSizeF(font.pointSizeF() + fSizeOffset);
    font.setWeight((int)eWeight);
    font.setItalic(bItalic);
    return font;
}

void CPlayerView::setFont(QWidget *widget, float fSizeOffset, E_FontWeight eWeight, bool bItalic) const
{
    if (widget)
    {
        widget->setFont(genFont(fSizeOffset, eWeight, bItalic));
    }
}

void CPlayerView::setTimer(UINT uMs, const function<bool()>& cb)
{
    QTimer::singleShot(uMs, [=](){
        if (cb())
        {
            setTimer(uMs, cb);
        }
    });
}
