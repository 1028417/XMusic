#pragma once

#include "dialog.h"

class CMsgBox : private CDialog
{
 public:
    CMsgBox();

public:
    void show(cqstr qsMsg, cfn_void cbClose);

private:
    cqcr bkgColor() const override;

    void _onPaint(CPainter& painter, cqrc rc) override;
};
