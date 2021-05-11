
#pragma once

#include <QPushButton>

#include "widget.h"

class CButton : public TWidget<QPushButton>
{
    Q_OBJECT
public:
    CButton(QWidget *parent) : TWidget(parent)
    {
        setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }

public:
    void performClick()
    {
        emit signal_clicked(this);
    }

    template <typename _slot>
    void onClicked(TD_XObj<_slot> recv, _slot slot)
    {
        onUISignal(&CButton::signal_clicked, recv, slot);
    }

    void onClicked(cfn_void fn)
    {
        //connect(this, &CButton::signal_clicked, fn);
        onUISignal(&CButton::signal_clicked, fn);
    }

    void connect_dlgClose(class CDialog *dlg);

private:
    bool m_bPressing = false;

signals:
    void signal_clicked(CButton*);

private:
    void _onPaint(CPainter&, cqrc) override;

    void _onMouseEvent(E_MouseEventType type, const QMouseEvent&) override;
};
