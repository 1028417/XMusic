
#pragma once

#include "dialog.h"

#include "listview.h"

/*class CAddBkgView : public CListView
{
};*/

class CAddBkgDlg : public CDialog<>
{
    Q_OBJECT
public:
    explicit CAddBkgDlg();

private:
    //CAddBkgView m_AddBkgView;

public:
    void _relayout(int cx, int cy);
};
