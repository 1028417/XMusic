
#pragma once

#include "dialog.h"

#include "listview.h"

/*class CFsView : public CListView
{
};*/

class CFsDlg : public CDialog<>
{
    Q_OBJECT
public:
    explicit CFsDlg();

public:
    void _relayout(int cx, int cy);
};
