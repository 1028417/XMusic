
#pragma once

using TD_Img = QImage;
//using TD_Img = QPixmap;

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

private:
    signal_t m_bRunSignal;

    QPixmap m_pmIcon;

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

    //void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;

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

    wstring displayName() const;// override;

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

template <class T=QPixmap>
static void zoomoutPixmap(T& pm, int cx, int cy, bool bCut)
{
    auto cxPm = pm.width();
    if (0 == cxPm)
    {
        return;
    }
    auto cyPm = pm.height();

    if (bCut)
    {
        if (cx > cy)
        {
            auto cyMax = cxPm*2/3;
            if (cyPm > cyMax)
            {
                T&& temp = pm.copy(0, (cyPm-cyMax)/2, cxPm, cyMax);
                pm.swap(temp);
                cyPm = cyMax;
            }
        }
        else
        {
            auto cxMax = cyPm*2/3;
            if (cxPm > cxMax)
            {
                T&& temp = pm.copy((cxPm-cxMax)/2, 0, cxMax, cyPm);
                pm.swap(temp);
                cxPm = cxMax;
            }
        }
    }

    if ((float)cyPm/cxPm > (float)cy/cx)
    {
        if (cxPm > cx)
        {
            T&& temp = pm.scaledToWidth(cx, Qt::SmoothTransformation);
            pm.swap(temp);
        }
    }
    else
    {
        if (cyPm > cy)
        {
            T&& temp = pm.scaledToHeight(cy, Qt::SmoothTransformation);
            pm.swap(temp);
        }
    }
}
