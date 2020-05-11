
#pragma once

#include "dialog.h"

#include "MLListView.h"

class IImgDir
{
public:
    IImgDir() = default;

    virtual ~IImgDir() = default;

    virtual wstring displayName() const = 0;

    virtual cqpm snapshot() const = 0;

    virtual size_t imgCount() const = 0;

    virtual const QPixmap* img(UINT uIdx) const = 0;

    virtual wstring imgPath(UINT uIdx) const = 0;

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
    size_t getColCount() const override;

    size_t getRowCount() const override;

    size_t getItemCount() const override;

    void _onPaintItem(CPainter&, tagLVItem&) override;

    void _onRowClick(tagLVItem& lvItem, const QMouseEvent&) override;

public:
    void showImgDir(IImgDir& imgDir);

    bool isInRoot() const
    {
        return NULL==m_pImgDir;
    }

    void upward();
};


class CAddBkgDlg : public CDialog
{
public:
    CAddBkgDlg(class CBkgDlg& bkgDlg, const TD_ImgDirList& paImgDir);

private:
    class CBkgDlg& m_bkgDlg;

    CAddBkgView m_lv;

    size_t m_uRowCount = 0;

private:
    void _relayout(int cx, int cy) override;

    bool _handleReturn() override;

public:
    size_t rowCount() const
    {
        return m_uRowCount;
    }

    class CBkgDlg& bkgDlg()
    {
        return m_bkgDlg;
    }

    void init();

    void show(cfn_void cbClose = NULL);

    void relayout()
    {
        _relayout(width(), height());
    }

    bool isInRoot() const
    {
        return m_lv.isInRoot();
    }
};
