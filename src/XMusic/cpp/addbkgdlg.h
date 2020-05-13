
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

    virtual void loadImg(int nIdx, const function<void(UINT, QPixmap&)>& cb) = 0;
    virtual bool genSubImgs() = 0;
};

using TD_ImgDirList = PtrArray<IImgDir>;

class CAddBkgView : public CListView
{
public:
    CAddBkgView(class CAddBkgDlg& addbkgDlg, class CApp& app, const TD_ImgDirList& paImgDir);

private:
    class CAddBkgDlg& m_addbkgDlg;
    class CApp& m_app;
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

class CImgDir : public CPath, public IImgDir
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

    UINT m_uPos = 0;

    QPixmap m_pmSnapshot;

    vector<tagBkgImg> m_vecImgs;

public:
    void clear()
    {
        m_uPos = 0;
        m_pmSnapshot = QPixmap();
        m_vecImgs.clear();

        CPath::clear();
    }

private:
    wstring displayName() const override;

    const cqpm snapshot() const override
    {
        return m_pmSnapshot;
    }

    size_t imgCount() const override
    {
        return m_vecImgs.size();
    }

    const QPixmap* img(UINT uIdx) const override;

    wstring imgPath(UINT uIdx) const override;

    void loadImg(int nIdx, const function<void(UINT, QPixmap&)>& cb) override;
    bool genSubImgs() override;

    CPath* _newSubDir(const tagFileInfo& fileInfo) override;

    XFile* _newSubFile(const tagFileInfo& fileInfo) override;
};

class CAddBkgDlg : public CDialog
{
public:
    CAddBkgDlg(class CBkgDlg& bkgDlg, class CApp& app);

private:
    class CBkgDlg& m_bkgDlg;
    class CApp& m_app;

    XThread& m_thrScan;
    CImgDir m_rootImgDir;
    TD_ImgDirList m_paImgDirs;

    CAddBkgView m_lv;

    size_t m_uRowCount = 0;

private:
    void _relayout(int cx, int cy) override;

    bool _handleReturn() override;    

    void _onClosed() override
    {
        m_lv.upward();
    }

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

    void show();

    void relayout()
    {
        _relayout(width(), height());
    }
};
