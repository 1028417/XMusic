
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

class CAddBkgView : public CListView
{
public:
    CAddBkgView(class CAddBkgDlg& addbkgDlg);

private:
    class CAddBkgDlg& m_addbkgDlg;

    CPath m_sdcard;

public:
    void scan();

    bool isInRoot() const
    {
        return false;
    }

    void upward() {}

private:
    UINT getRowCount() override;

    void _onPaintRow(CPainter&, QRect&, const tagLVRow&) override;
};

class CAddBkgDlg : public CDialog<>
{
    Q_OBJECT
public:
    explicit CAddBkgDlg();

private:
    CAddBkgView m_addbkgView;

    std::thread m_thread;

private:
    void _relayout(int cx, int cy);

    bool _handleReturn();

public:
    void init();

    void show();
};
