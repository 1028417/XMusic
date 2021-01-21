
#pragma once

#include "widget/listview.h"

class CBkgView : public CListView
{
public:
    CBkgView(class CBkgDlg& bkgDlg);

private:    
    class CBkgDlg& m_bkgDlg;

    QPixmap m_pmX;

    int m_szButton = 0;

public:
    void init()
    {
        (void)m_pmX.load(__png(btnX));
    }

    UINT margin();

    void setButtonSize(int szButton)
    {
        m_szButton = szButton;
    }

private:
    size_t getRowCount() const override;

    size_t getColCount() const override;

    size_t getItemCount() const override;

    void _onPaintItem(CPainter&, tagLVItem&) override;

    void _onItemClick(tagLVItem&, const QMouseEvent&) override;
};
