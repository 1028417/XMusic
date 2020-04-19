
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

class IImgDir
{
public:
    IImgDir() = default;

    virtual ~IImgDir() = default;

    virtual size_t imgCount() const = 0;

    virtual const QPixmap* snapshot(int nIdx=-1) const = 0;

    virtual wstring path(int nIdx=-1) const = 0;

    virtual bool genSubImgs() = 0;
};

using TD_ImgDirList = PtrArray<IImgDir>;

class CAddBkgView : public CListView
{
public:
    CAddBkgView(class CAddBkgDlg& addbkgDlg, const TD_ImgDirList& paImgDir);

private:
    class CAddBkgDlg& m_addbkgDlg;

    const TD_ImgDirList& m_paImgDirs;

    IImgDir *m_pImgDir = NULL;

private:
    void _reset() override
    {
        m_pImgDir = NULL;
    }

    size_t getColumnCount() const override;

    size_t getPageRowCount() const override;

    size_t getRowCount() const override;

    void _onPaintRow(CPainter&, tagLVRow&) override;

    void _onRowClick(tagLVRow& lvRow, const QMouseEvent&) override;

public:
    void showImgDir(IImgDir& imgDir);

    bool isInRoot() const
    {
        return NULL==m_pImgDir;
    }

    bool upward();
};


class CAddBkgDlg : public CDialog
{
public:
    CAddBkgDlg(class CBkgDlg& bkgDlg, const TD_ImgDirList& paImgDir);

private:
    class CBkgDlg& m_bkgDlg;

    CAddBkgView m_lv;

private:
    void _relayout(int cx, int cy) override;

    bool _handleReturn() override
    {
        return m_lv.upward();
    }

public:
    class CBkgDlg& bkgDlg()
    {
        return m_bkgDlg;
    }

    void init();

    void show(IImgDir *pImgDir = NULL, cfn_void cbClose = NULL);

    void relayout()
    {
        _relayout(width(), height());
    }
};
