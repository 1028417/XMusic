
#include <QApplication>

#include "mainwindow.h"

#include "view.h"

#include <QTimer>

IModelObserver& CPlayerView::getModelObserver()
{
    return m_mainWnd;
}

QFont CPlayerView::genFont(double dbOffsetSize, bool bItalic, E_FontWeight eWeight) const
{
    QFont font(m_app.font());
    font.setPointSizeF(font.pointSizeF() + dbOffsetSize);
    font.setWeight((int)eWeight);
    font.setItalic(bItalic);
    return font;
}

void CPlayerView::setFont(QWidget *widget, double dbOffsetSize, bool bItalic, E_FontWeight eWeight) const
{
    if (widget)
    {
        widget->setFont(genFont(dbOffsetSize, bItalic, eWeight));
    }
}

void CPlayerView::setTextColor(QWidget *widget, const QColor& crText)
{
    QPalette pe = widget->palette();
    pe.setColor(QPalette::WindowText, crText);
    widget->setPalette(pe);
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
