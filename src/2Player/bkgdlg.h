
#pragma once

#include <QDialog>

class CBkgDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CBkgDlg(QWidget *parent = 0);

private:
    bool event(QEvent *) override;

    void _relayout();
};
