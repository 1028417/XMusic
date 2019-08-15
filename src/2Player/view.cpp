
#include <QApplication>

#include "mainwindow.h"

#include "view.h"

#include <QTimer>

IModelObserver& CPlayerView::getModelObserver()
{
    return m_mainWnd;
}

extern map<E_FontWeight, QFont> g_mapFont;

QFont CPlayerView::genFont(double dbOffsetSize, E_FontWeight eWeight, bool bItalic) const
{
    QFont font = g_mapFont[eWeight];
    font.setPointSizeF(font.pointSizeF() + dbOffsetSize);
    font.setWeight((int)eWeight);
    font.setItalic(bItalic);
    return font;
}

void CPlayerView::setFont(QWidget *widget, double dbOffsetSize, E_FontWeight eWeight, bool bItalic) const
{
    if (widget)
    {
        widget->setFont(genFont(dbOffsetSize, eWeight, bItalic));
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
