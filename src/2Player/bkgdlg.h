
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

    void _onPaintItem(CPainter&, QRect&, const tagListViewRow&) override;
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

    vector<WString>& bkgs()
    {
        return m_bHScreen?m_vecHBkg:m_vecVBkg;
    }

    const WString& bkgDir() const
    {
        return m_bHScreen?m_strHBkgDir:m_strVBkgDir;
    }

private:
    void _relayout(int cx, int cy) override;
};
