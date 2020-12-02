
#pragma once

#include "dialog.h"

struct tagBkgImg
{
    tagBkgImg() = default;

    tagBkgImg(QPixmap& pm, cwstr strPath)
        : strPath(strPath)
    {
        this->pm.swap(pm);
    }

    QPixmap pm;
    wstring strPath;
};

class CImgDir : public CPath
{
public:
    CImgDir(XT_RunSignal bRunSignal)
        : m_bRunSignal(bRunSignal)
    {
    }

    CImgDir(XT_RunSignal bRunSignal, const tagFileInfo& fileInfo)
        : CPath(fileInfo)
        , m_bRunSignal(bRunSignal)
    {
    }

private:
    XT_RunSignal m_bRunSignal;

    QPixmap m_pmSnapshot;

    UINT m_uPos = 0;
    vector<tagBkgImg> m_vecImgs;

public:
    void clear()
    {
        m_pmSnapshot = QPixmap();

        m_uPos = 0;
        m_vecImgs.clear();

        CPath::clear();
    }

    void cleanup()
    {
        m_uPos = 0;
        m_vecImgs.clear();
    }

private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override;
    XFile* _newSubFile(const tagFileInfo& fileInfo) override;

    bool _genSubImgs(class CAddBkgView& lv);

    void _genSubImgs(cwstr strFile, QPixmap& pm);

public:
    wstring displayName() const;

    cqpm snapshot() const
    {
        return m_pmSnapshot;
    }

    size_t imgCount() const
    {
        return m_vecImgs.size();
    }

    const QPixmap* img(UINT uIdx) const;

    wstring imgPath(UINT uIdx) const;

    void genSubImgs(class CAddBkgView& lv);
};

using TD_ImgDirList = PtrArray<CImgDir>;

class CAddBkgView : public CListView
{
public:
    CAddBkgView(class CAddBkgDlg& addbkgDlg, const TD_ImgDirList& paImgDir);

private:
    class CAddBkgDlg& m_addbkgDlg;

    const TD_ImgDirList& m_paImgDirs;

    CImgDir *m_pImgDir = NULL;

private:
    size_t getColCount() const override;

    size_t getRowCount() const override;

    size_t getItemCount() const override;

    void _onPaintItem(CPainter&, tagLVItem&) override;

    void _onItemClick(tagLVItem& lvItem, const QMouseEvent&) override;

    void _showImgDir(CImgDir& imgDir);

public:
    CImgDir* imgDir() const
    {
        return m_pImgDir;
    }

    bool handleReturn();
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
    bool _chooseDir();
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
