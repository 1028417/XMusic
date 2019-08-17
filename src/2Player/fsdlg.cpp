#include "view.h"

#include "fsdlg.h"
#include "ui_fsdlg.h"

static Ui::FsDlg ui;

CFsDlg::CFsDlg()
{
    ui.setupUi(this);

    connect(ui.btnReturn, &CButton::signal_clicked, [&](CButton*) {
        this->close();
    });
}
