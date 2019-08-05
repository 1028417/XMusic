
#pragma once

#include <QPushButton>

#include <QGraphicsOpacityEffect>

#include "widget.h"

class CButton : public CWidget<QPushButton>
{
    Q_OBJECT

public:
    CButton(QWidget *parent) :
        CWidget(parent, {Qt::TapAndHoldGesture})
    {
        setFocusPolicy(Qt::FocusPolicy::NoFocus);

        connect(this, SIGNAL(clicked()), this, SLOT(slot_clicked()));
    }

signals:
    void signal_clicked(CButton*);

    void signal_contextMenu(CButton*);

private slots:
    void slot_clicked()
    {
        emit signal_clicked(this);
    }

private:
    void mousePressEvent(QMouseEvent *ev) override
    {
        QGraphicsOpacityEffect ge;
        ge.setOpacity(0.4);
        this->setGraphicsEffect(&ge);

        QPushButton::mousePressEvent(ev);
    }

    void _onGesture(QGesture&) override
    {
        emit signal_contextMenu(this);
    }
};
