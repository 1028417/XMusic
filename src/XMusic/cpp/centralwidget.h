
#pragma once

#include "widget.h"

#include "mainwindow.h"

class CCentralWidget : public TWidget<QWidget>
{
public:
    CCentralWidget(QWidget *parent) : TWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

    void _onTouchEvent(E_TouchEventType tet, const CTouchEvent& te) override
    {
        if (E_TouchEventType::TET_TouchMove == tet)
        {
            ((MainWindow*)parent())->handleTouchMove(te);
        }
    }
};
