
#pragma once

#include <QProgressBar>

#include "widget.h"

class CColorBar : public CWidget<QProgressBar>
{
public:
    CColorBar(QWidget *parent) : CWidget<QProgressBar>(parent)
    {
    }
};
