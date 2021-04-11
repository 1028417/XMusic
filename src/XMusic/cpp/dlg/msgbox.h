#pragma once

#include "dialog.h"

class CMsgBox : private CDialog
{
 public:
    CMsgBox();

public:
    void show(cqstr qsMsg, cfn_void cbClose);

private:
    void _setupUi();

    void _setPos() override;

    void _relayout(int cx, int cy) override;

    void _onPaint(CPainter&, cqrc) override;
};
