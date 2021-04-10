#pragma once

#include "dialog.h"

class CMsgBox : private CDialogEx
{
 public:
    CMsgBox() = default;

public:
    void show(cqstr qsMsg, cfn_void cbClose);

private:
    void _setupUi();

    cqcr bkgColor() const override;

    void _onPaint(CPainter&, cqrc) override;
};
