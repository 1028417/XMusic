
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

    vector<WString> m_vecHBkgFile;
    vector<WString> m_vecVBkgFile;

    list<QPixmap> m_lstSnapshot;
    vector<const QPixmap*> m_vecHSnapshot;
    vector<const QPixmap*> m_vecVSnapshot;

    QPixmap m_pmHBkg;
    QPixmap m_pmVBkg;

    bool m_bHScreen = false;

private:
    inline WString& _bkgDir()
    {
        return m_bHScreen?m_strHBkgDir:m_strVBkgDir;
    }

    inline vector<WString>& _vecBkgFile()
    {
        return m_bHScreen?m_vecHBkgFile:m_vecVBkgFile;
    }

    inline vector<const QPixmap*>& _vecSnapshot()
    {
        return m_bHScreen?m_vecHSnapshot:m_vecVSnapshot;
    }

    void _relayout(int cx, int cy) override;

    void _setBkg(const wstring& strFile);

    void _onClose() override;

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
        (m_bHScreen?m_vecHBkgFile:m_vecVBkgFile).size()+1;
    }

    const QPixmap* snapshot(size_t uIdx);

    void setBkg(size_t uIdx);

    void addBkg(const wstring& strFile);

    void deleleBkg(size_t uIdx);
};
