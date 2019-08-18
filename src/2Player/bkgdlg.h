
#pragma once

#include "dialog.h"

#include "listview.h"

#include "addbkgdlg.h"

class CBkgView : public CListView
{
public:
    CBkgView(class CBkgDlg& bkgDlg);

private:
    class CBkgDlg& m_bkgDlg;

private:
    UINT getRowCount() override;

    void _onPaintRow(CPainter&, QRect&, const tagListViewRow&) override;

    void _handleRowClick(const tagListViewRow&, const QMouseEvent&) override;
};

class CBkgDlg : public CDialog<>
{
    Q_OBJECT
public:
    explicit CBkgDlg(class CPlayerView& view);

private:
    class CPlayerView& m_view;

    CBkgView m_bkgView;

    CAddBkgDlg m_fsDlg;

    WString m_strHBkgDir;
    WString m_strVBkgDir;

    vector<WString> m_vecHBkg;
    vector<WString> m_vecVBkg;

    bool m_bHScreen = false;

    QPixmap m_pmDefaultBkg;

    QPixmap m_pmHBkg;
    QPixmap m_pmVBkg;

public:    
    void init();

    void setDefaultBkg(const QPixmap& pmDefaultBkg)
    {
        m_pmDefaultBkg = pmDefaultBkg;
    }

    const QPixmap& defaultBkg() const
    {
        return m_pmDefaultBkg;
    }

    const QPixmap& hbkg() const
    {
        return m_pmHBkg;
    }

    const QPixmap& vbkg() const
    {
        return m_pmVBkg;
    }

    UINT bkgCount()
    {
        if (m_bHScreen)
        {
            return m_vecHBkg.size();
        }
        else
        {
            return m_vecVBkg.size();
        }
    }

    void paintDefaultBkg(QPainter& painter, const QRect& rcDst);

    WString bkg(UINT uIdx);

    void setBkg(UINT uIdx);
    void unsetBkg();

    bool addBkg();

private:
    void _relayout(int cx, int cy) override;
};
