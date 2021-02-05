
#pragma once

//using TD_Img = QImage;
using TD_Img = QPixmap;

/*class IImgDir
{
public:
    IImgDir() = default;

    virtual ~IImgDir() = default;

    virtual void cleanup() = 0;

    virtual wstring displayName() const = 0;

    virtual cqpm icon() const = 0;

    virtual size_t imgCount() const = 0;

    virtual const TD_Img* img(UINT uIdx) const = 0;

    virtual wstring imgPath(UINT uIdx) const = 0;

    virtual void genSubImgs(class CAddBkgView& lv) = 0;
};*/

#define IImgDir CImgDir

class CImgDir : public CPath//, public IImgDir
{
private:
    struct tagBkgImg
    {
        tagBkgImg() = default;

        tagBkgImg(TD_Img& pm, cwstr strPath)
            : strPath(strPath)
        {
            this->pm.swap(pm);
        }

        TD_Img pm;
        wstring strPath;
    };

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

protected:
    signal_t m_bRunSignal;

    QPixmap m_pmIcon;

private:
    UINT m_uPos = 0;
    vector<tagBkgImg> m_vecImgs;

protected:
    bool _genIcon(cwstr strFile);

private:
/*#if __android
    CImgDir(signal_t bRunSignal, cwstr strDir)
        : CPath(strDir)
        , m_bRunSignal(bRunSignal)
    {
    }

    virtual void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;
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

    CPath* _newSubDir(const tagFileInfo& fileInfo) override;
    XFile* _newSubFile(const tagFileInfo& fileInfo) override;

    bool _genSubImgs(class CAddBkgView& lv);

    void _genSubImgs(cwstr strFile, TD_Img& pm);

public:
    /*void clear()
    {
        m_pmIcon = QPixmap();

        m_uPos = 0;
        m_vecImgs.clear();

        CPath::clear();
    }*/

    void cleanup()// override
    {
        m_uPos = 0;
        m_vecImgs.clear();
    }

    virtual wstring displayName() const;// override;

    cqpm icon() const// override
    {
        return m_pmIcon;
    }

    size_t imgCount() const// override
    {
        return m_vecImgs.size();
    }

    const TD_Img* img(UINT uIdx) const;// override;

    wstring imgPath(UINT uIdx) const;// override;

    void genSubImgs(class CAddBkgView& lv);// override;
};

class COlBkgDir : public CImgDir
{
public:
    COlBkgDir(signal_t bRunSignal)
        : CImgDir(bRunSignal)
        , m_olBkg(s_olBkg)
    {
    }

    COlBkgDir(signal_t bRunSignal, const tagFileInfo& fileInfo, const struct tagOlBkg& olBkg);

private:
    static const struct tagOlBkg s_olBkg;
    const struct tagOlBkg& m_olBkg;

private:
    wstring displayName() const override
    {
        return fileName();
    }

    void _onFindFile(TD_PathList&, TD_XFileList&) override {}

public:
    void tryAdd(COlBkgDir& dir);

    string url(XFile& file);
};

void zoomoutPixmap(QPixmap& pm, int cx, int cy, bool bCut);
void zoomoutPixmap(QImage& img, int cx, int cy, bool bCut);
