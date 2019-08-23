
#include <QApplication>

#include "mainwindow.h"

#include "view.h"

#include <QTimer>

IModelObserver& CPlayerView::getModelObserver()
{
    return m_mainWnd;
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
