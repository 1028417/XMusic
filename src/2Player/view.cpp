
#include <QApplication>

#include "mainwindow.h"

#include "view.h"

#include <QTimer>

IModelObserver& CPlayerView::getModelObserver()
{
    return m_mainWnd;
}

//QFont CPlayerView::font() const
//{
//    return m_app.font();
//}

QFont CPlayerView::genFont(double dbOffsetSize, bool bBold, bool bItalic) const
{
    QFont font(m_app.font());
    font.setPointSizeF(font.pointSizeF() + dbOffsetSize);
    font.setBold(bBold);
    font.setItalic(bItalic);
    return font;
}

void CPlayerView::setFont(QWidget *widget, double dbOffsetSize, bool bBold, bool bItalic) const
{
    if (widget)
    {
        widget->setFont(genFont(dbOffsetSize, bBold, bItalic));
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
