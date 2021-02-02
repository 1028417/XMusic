
#pragma once

#include "dlg/dialog.h"

#include "imgdir.h"

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

    void _onItemClick(tagLVItem& lvItem, const QMouseEvent&) override;

    void _showImgDir(IImgDir& imgDir);

public:
    IImgDir* imgDir() const
    {
        return m_pImgDir;
    }

    void handleReturn(bool bClose);
};

class CAddBkgDlg : public CDialog
{
public:
    CAddBkgDlg(class CBkgDlg& bkgDlg);

private:
    class CBkgDlg& m_bkgDlg;

    XThread& m_thrScan;
    CImgDir m_rootImgDir;
    TD_ImgDirList m_paImgDirs;

    CAddBkgView m_lv;

    size_t m_uRowCount = 0;

private:
    void _relayout(int cx, int cy) override;

    bool _handleReturn() override;    

#if __windows
    wstring _chooseDir();
#endif

    void _scanDir(cwstr strDir);

public:
    size_t rowCount() const
    {
        return m_uRowCount;
    }

    void init();

    void show();

    void relayout()
    {
        _relayout(width(), height());
    }

    void addBkg(cwstr strFile);
};
