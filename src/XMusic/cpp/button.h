
#pragma once

#include <QPushButton>

#include "widget.h"

class CButton : public CWidget<QPushButton>
{
    Q_OBJECT
public:
    CButton(QWidget *parent) : CWidget(parent)
    {
        setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }

signals:
    void signal_clicked(CButton*);

    //void signal_contextMenu(CButton*);

private:
    bool event(QEvent *ev) override;

    void _onMouseEvent(E_MouseEventType type, const QMouseEvent&) override;

    /*bool _onGesture(QGesture&) override
    {
        emit signal_contextMenu(this);
        return true;
    }*/
};
