
#pragma once

#include "widget.h"

#include "app.h"

#include "mainwindow.h"

class CCentralWidget : public TWidget<QWidget>
{
public:
    CCentralWidget(QWidget *parent) : TWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

    void _onTouchEvent(E_TouchEventType type, const CTouchEvent& te) override
    {
        if (E_TouchEventType::TET_TouchMove == type)
        {
            ((MainWindow*)parent())->handleTouchMove(te);
        }
    }
};
