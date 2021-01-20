
#pragma once

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

private:
/*#if __android
    CImgDir(signal_t bRunSignal, cwstr strDir)
        : CPath(strDir)
        , m_bRunSignal(bRunSignal)
    {
    }
#endif*/

    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;

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

    wstring displayName() const override;

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
