
#pragma once

#include "dialog.h"

#include "widget/listview.h"

#include "addbkgdlg.h"

#include "colordlg.h"

class CBkgView : public CListView
{
public:
    CBkgView(class CBkgDlg& bkgDlg);

private:    
    class CBkgDlg& m_bkgDlg;

    QPixmap m_pmX;

public:
    void init()
    {
        (void)m_pmX.load(__png(btnX));
    }

    UINT margin();

private:
    size_t getRowCount() const override;

    size_t getColCount() const override;

    size_t getItemCount() const override;

    void _onPaintItem(CPainter&, tagLVItem&) override;

    void _onItemClick(tagLVItem&, const QMouseEvent&) override;
};

class CBkgBrush : public QBrush
{
public:
    CBkgBrush()=default;

    CBkgBrush(cqpm pm)
        : QBrush(pm)
    {
        m_cx = pm.width();
        m_cy = pm.height();
    }

    int m_cx = 0;
    int m_cy = 0;
};

struct tagBkgFile
{
    tagBkgFile() = default;

    tagBkgFile(bool bInner, cwstr strFile, CBkgBrush *br=NULL)
        : bInner(bInner)
        , strFile(strFile)
        , br(br)
    {
    }

    bool bInner = false;
    WString strFile;
    CBkgBrush *br = NULL;
};

class CBkgDlg : public CDialog
{
    Q_OBJECT
public:
    CBkgDlg(QWidget& parent);

private:
    struct tagOption& m_option;

    CBkgView m_lv;

    wstring m_strHBkgDir;
    wstring m_strVBkgDir;

    vector<tagBkgFile> m_vecHBkgFile;
    vector<tagBkgFile> m_vecVBkgFile;

    list<CBkgBrush> m_lstBr;

    QPixmap m_pmHBkg;
    QPixmap m_pmVBkg;

    CAddBkgDlg m_addbkgDlg;

    CColorDlg m_colorDlg;

private:
    inline wstring& _bkgDir()
    {
        return m_bHLayout?m_strHBkgDir:m_strVBkgDir;
    }

    inline vector<tagBkgFile>& _vecBkgFile()
    {
        return m_bHLayout?m_vecHBkgFile:m_vecVBkgFile;
    }

    CBkgBrush& _addbr(QPixmap& pm, bool bHLayout);

    void _relayout(int cx, int cy) override;

    bool _setBkg(int nIdx);

    void _updateBkg(cwstr strFile);

    void _onClosed() override;

public:
    static size_t caleRowCount(int cy);

    void preinitBkg(bool bHLayout);

    void init();

    cqpm hbkg() const
    {
        return m_pmHBkg;
    }
    cqpm vbkg() const
    {
        return m_pmVBkg;
    }

    size_t bkgCount() const;

    CBkgBrush* brush(size_t uIdx);

    void switchBkg(bool bHLayout, bool bNext);

    void handleLVClick(size_t uItem);

    void deleleBkg(size_t uIdx);

    void addBkg(cwstr strFile);
};
