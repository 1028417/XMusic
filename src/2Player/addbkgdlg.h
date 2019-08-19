
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

class CAddBkgView : public CListViewEx
{
public:
    CAddBkgView(class CAddBkgDlg& addbkgDlg);

private:
    class CAddBkgDlg& m_addbkgDlg;

public:
    UINT getRootCount() override;

    bool _genRootRowContext(const tagLVRow&, tagRowContext&) override;
};

class CAddBkgDlg : public CDialog<>
{
    Q_OBJECT
public:
    explicit CAddBkgDlg();

private:
    CAddBkgView m_addbkgView;

private:
    void _relayout(int cx, int cy);

    bool _handleReturn();

public:
    void init();
};
