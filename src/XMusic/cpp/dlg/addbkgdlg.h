
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

class IImgDir
{
public:
    IImgDir() = default;

    virtual ~IImgDir() = default;

    virtual void cleanup() = 0;

    virtual wstring displayName() const = 0;

    virtual cqpm snapshot() const = 0;

    virtual size_t imgCount() const = 0;

    virtual const QPixmap* img(UINT uIdx) const = 0;

    virtual wstring imgPath(UINT uIdx) const = 0;

    virtual void genSubImgs(class CAddBkgView& lv) = 0;
};

class CImgDir : public CPath, public IImgDir
{
public:
    CImgDir(signal_t bRunSignal)
        : m_bRunSignal(bRunSignal)
    {
    }

    CImgDir(signal_t bRunSignal, const tagFileInfo& fileInfo)
        : CPath(fileInfo)
        , m_bRunSignal(bRunSignal)
    {
    }

private:
    signal_t m_bRunSignal;

    QPixmap m_pmSnapshot;

    UINT m_uPos = 0;
    vector<tagBkgImg> m_vecImgs;

/*#if __android
private:
    CImgDir(signal_t bRunSignal, cwstr strDir)
        : CPath(strDir)
        , m_bRunSignal(bRunSignal)
    {
    }

    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override
    {
        if (__sdcardDir == m_fi.strName)
        {
            auto strRoot = L"/storage/";
            (void)fsutil::findSubDir(strRoot, [&](cwstr strSubDir) {
                paSubDir.addFront(new CImgDir(m_bRunSignal, strRoot + strSubDir));
            });
        }

        CPath::_onFindFile(paSubDir, paSubFile);
    }
#endif*/

private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override;
    XFile* _newSubFile(const tagFileInfo& fileInfo) override;

    bool _genSubImgs(class CAddBkgView& lv);

    void _genSubImgs(cwstr strFile, QPixmap& pm);

public:
    /*void clear()
    {
        m_pmSnapshot = QPixmap();

        m_uPos = 0;
        m_vecImgs.clear();

        CPath::clear();
    }*/

    void cleanup() override
    {
        m_uPos = 0;
        m_vecImgs.clear();
    }

    wstring displayName() const;

    cqpm snapshot() const override
    {
        return m_pmSnapshot;
    }

    size_t imgCount() const override
    {
        return m_vecImgs.size();
    }

    const QPixmap* img(UINT uIdx) const override;

    wstring imgPath(UINT uIdx) const override;

    void genSubImgs(class CAddBkgView& lv) override;
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
