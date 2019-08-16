
#pragma once

#include "dialog.h"

#include "listview.h"

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

public:
    void init();
};

class CBkgDlg : public CDialog<>
{
    Q_OBJECT
public:
    explicit CBkgDlg(class CPlayerView& view);

private:
    class CPlayerView& m_view;

    CBkgView m_bkgView;

    WString m_strHBkgDir;
    WString m_strVBkgDir;

    vector<WString> m_vecHBkg;
    vector<WString> m_vecVBkg;

    bool m_bHScreen = false;

public:
    void init();

    WString hbkg() const;
    WString vbkg() const;

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

    WString bkg(UINT uIdx);

    bool addBkg();

    void setBkg(UINT uIdx);
    void unsetBkg();

private:
    void _relayout(int cx, int cy) override;
};
