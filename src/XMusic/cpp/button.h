
#pragma once

#include <QPushButton>

#include "widget.h"

class CButton : public CWidget<QPushButton>
{
    Q_OBJECT
public:
    CButton(QWidget *parent) : CWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);

        setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }

private:
    bool m_bPressing = false;

signals:
    void signal_clicked(CButton*);

private:
    bool event(QEvent *ev) override;

    void _onMouseEvent(E_MouseEventType type, const QMouseEvent&) override;
};
