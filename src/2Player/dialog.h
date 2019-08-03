#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class CDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CDialog(QWidget *parent = 0);
    ~CDialog();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
