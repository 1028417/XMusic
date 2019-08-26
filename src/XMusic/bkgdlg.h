
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
    UINT getPageRowCount() override;

    UINT getColumnCount() override;

    UINT getRowCount() override;

    void _onPaintRow(CPainter&, QRect&, const tagLVRow&) override;

    void _onRowClick(const tagLVRow&, const QMouseEvent&) override;
};

class CBkgDlg : public CDialog
{
    Q_OBJECT
public:
    CBkgDlg(class CPlayerView& view) :
        m_view(view),
        m_bkgView(*this),
        m_addbkgDlg(view, *this)
    {
    }

private:
    class CPlayerView& m_view;

    CBkgView m_bkgView;

    CAddBkgDlg m_addbkgDlg;

    WString m_strHBkgDir;
    WString m_strVBkgDir;

    vector<WString> m_vecHBkg;
    vector<WString> m_vecVBkg;

    list<QPixmap> m_lstSnapshot;
    vector<const QPixmap*> m_vecHSnapshot;
    vector<const QPixmap*> m_vecVSnapshot;

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

    UINT bkgCount() const
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

    const QPixmap* snapshot(UINT uIdx);

    void setBkg(UINT uIdx);
    void unsetBkg();

    bool addBkg();

private:
    void _relayout(int cx, int cy) override;

    void _onClose() override
    {
        m_vecHSnapshot.clear();
        m_vecVSnapshot.clear();
        m_lstSnapshot.clear();

        m_bkgView.reset();
    }
};
