
#pragma once

#include "dialog.h"

#include "listview.h"

#include "addbkgdlg.h"

#include "colordlg.h"

class CBkgView : public CListView
{
public:
    CBkgView(class CApp& app, class CBkgDlg& bkgDlg);

private:
    class CApp& m_app;

    class CBkgDlg& m_bkgDlg;

    QPixmap m_pmX;

private:
    size_t getPageRowCount() override;

    size_t getColumnCount() override;

    size_t getRowCount() override;

    void _onPaintRow(CPainter&, tagLVRow&) override;

    void _onRowClick(tagLVRow&, const QMouseEvent&) override;

public:
    UINT margin();
};

class CBkgDlg : public CDialog
{
    Q_OBJECT
public:
    CBkgDlg(class CApp& app);

private:
    class CApp& m_app;

    CColorDlg m_colorDlg;

    CAddBkgDlg m_addbkgDlg;

    CBkgView m_bkgView;

    WString m_strHBkgDir;
    WString m_strVBkgDir;

    vector<WString> m_vecHBkg;
    vector<WString> m_vecVBkg;

    QPixmap m_pmHBkg;
    QPixmap m_pmVBkg;

    bool m_bHScreen = false;

    list<QPixmap> m_lstSnapshot;
    vector<const QPixmap*> m_vecHSnapshot;
    vector<const QPixmap*> m_vecVSnapshot;

public:
    void init();

    void show();

    const QPixmap& hbkg() const
    {
        return m_pmHBkg;
    }

    const QPixmap& vbkg() const
    {
        return m_pmVBkg;
    }

    size_t bkgCount() const
    {
        if (m_bHScreen)
        {
            return m_vecHBkg.size()+1;
        }
        else
        {
            return m_vecVBkg.size()+1;
        }
    }

    const QPixmap* snapshot(size_t uIdx);

    void setBkg(size_t uIdx);

    void addBkg(const wstring& strFile);

    void deleleBkg(size_t uIdx);

private:
    void _relayout(int cx, int cy) override;

    void _setBkg(const wstring& strBkg);

    void _onClose() override;
};
