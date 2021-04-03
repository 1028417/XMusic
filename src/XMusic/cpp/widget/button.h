
#pragma once

#include <QPushButton>

#include "widget.h"

class CButton : public TWidget<QPushButton>
{
    Q_OBJECT
public:
    CButton(QWidget *parent) : TWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);

        setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }

private:
    bool m_bPressing = false;

signals:
    void signal_clicked(CButton*);

private:
    void _onPaint(CPainter& painter, cqrc rc) override;

    void _onMouseEvent(E_MouseEventType type, const QMouseEvent&) override;
};
