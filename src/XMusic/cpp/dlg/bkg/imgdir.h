
#pragma once

//using TD_Img = QImage;
using TD_Img = QPixmap;

class CImgDir : public CPath//, public CImgDir
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

private:
    signal_t m_bRunSignal;

    QPixmap m_pmIcon;

    UINT m_uPos = 0;

    vector<UINT> m_vecHPos;
    vector<UINT> m_vecVPos;

    vector<tagBkgImg> m_vecHImgs;
    vector<tagBkgImg> m_vecVImgs;

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
        m_vecHPos.clear();
        m_vecVPos.clear();
        m_vecHImgs.clear(); // m_vecHImgs = vector<tagBkgImg>();
        m_vecVImgs.clear(); // m_vecVImgs = vector<tagBkgImg>();
    }

    virtual wstring displayName() const;

    cqpm icon() const
    {
        return m_pmIcon;
    }

    size_t imgCount(bool bHLayout) const
    {
        return (bHLayout?m_vecHImgs:m_vecVImgs).size();
    }

    const TD_Img* img(bool bHLayout, UINT uIdx) const;

    wstring imgPath(bool bHLayout, UINT uIdx) const;

    bool genSubImg(class CAddBkgView& lv, UINT uGenCount);
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
    void tryAdd(COlBkgDir& dir, class CAddBkgView& lv);

    string url(XFile& file);
};

void zoomoutPixmap(QPixmap& pm, int cx, int cy,  bool bCut);
void zoomoutPixmap(QImage& img, int cx, int cy, bool bCut);
