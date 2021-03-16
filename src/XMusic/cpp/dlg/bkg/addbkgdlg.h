
#pragma once

#include "dlg/dialog.h"

#include "imgdir.h"

using TD_ImgDirList = PtrArray<CImgDir>;

class CAddBkgView : public CListView
{
public:
    CAddBkgView(class CAddBkgDlg& addbkgDlg);

private:
    class CAddBkgDlg& m_addbkgDlg;

    XThread& m_thrScan;
    CImgDir m_rootImgDir;

    COlBkgDir m_olBkgDir;

    TD_ImgDirList m_paImgDirs;

    CImgDir *m_pImgDir = NULL;

    QPixmap m_pmOlBkg;

private:
    size_t getColCount() const override;

    size_t getRowCount() const override;

    size_t getItemCount() const override;

    void _onPaintItem(CPainter&, tagLVItem&) override;
    void _paintRow(CPainter& painter, tagLVItem& lvItem, CImgDir& imgDir);

    void _onItemClick(tagLVItem& lvItem, const QMouseEvent&) override;

    void _showImgDir(CImgDir& imgDir);

public:    
    XThread& thrScan()
    {
        return m_thrScan;
    }

    void scanDir(cwstr strDir, class QLabel *labelLoading);

    UINT displayMode() const
    {
        if (m_pImgDir)
        {
            if (m_pImgDir == &m_olBkgDir)
            {
                return 0;
            }
            return 1;
        }
        return 0;
    }

    bool inRoot() const
    {
        return NULL == m_pImgDir;
    }

    CImgDir* imgDir() const
    {
        if (m_pImgDir == &m_olBkgDir)
        {
            return NULL;
        }
        return m_pImgDir;
    }

    bool handleReturn(bool bClose);

    bool isHLayout() const;
};

class CAddBkgDlg : public CDialog
{
public:
    CAddBkgDlg(class CBkgDlg& bkgDlg);

private:
    class CBkgDlg& m_bkgDlg;

    CAddBkgView m_lv;

    size_t m_uRowCount = 0;

private:
    void _relayout(int cx, int cy) override;

    bool _handleReturn() override;    

#if __windows
    wstring _chooseDir();
#endif
public:
    size_t rowCount() const
    {
        return m_uRowCount;
    }

    void init();

    void show();

    void onShowImgDir();

    void addBkg(cwstr strFile);
};
