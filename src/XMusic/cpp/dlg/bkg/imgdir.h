
#pragma once

#include "model/model.h"

//using TD_Img = QImage;
using TD_Img = QPixmap;

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

class CImgDir : public CPath//, public CImgDir
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

protected:
    CImgDir()
        : m_bRunSignal(__bRunSignal)
    {
    }

    CImgDir(const tagFileInfo& fileInfo)
        : CPath(fileInfo)
        , m_bRunSignal(__bRunSignal)
    {
    }

private:
    bool __bRunSignal = false;
    signal_t m_bRunSignal;

    UINT m_uPos = 0;

    list<wstring> m_lstHFile;
    list<wstring> m_lstVFile;

    SArray<tagBkgImg> m_vecHImg;
    SArray<tagBkgImg> m_vecVImg;

protected:
    QPixmap m_pmIcon;

protected:
    bool _genIcon(cwstr strFile);

    inline SArray<tagBkgImg>& _vecImg(bool bHLayout)
    {
        return bHLayout?m_vecHImg:m_vecVImg;
    }
    inline const SArray<tagBkgImg>& _vecImg(bool bHLayout) const
    {
        return bHLayout?m_vecHImg:m_vecVImg;
    }

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

    virtual wstring _genSubImg(class CAddBkgView&, XThread&, bool bHLayout, TD_Img&);

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
        m_lstHFile.clear();
        m_lstVFile.clear();
        m_vecHImg.clear(); // m_vecHImg = vector<tagBkgImg>();
        m_vecVImg.clear(); // m_vecVImg = vector<tagBkgImg>();
    }

    virtual wstring displayName() const;

    cqpm icon() const
    {
        return m_pmIcon;
    }

    bool genSubImg(class CAddBkgView&, XThread&);

    size_t imgCount(bool bHLayout) const
    {
        return _vecImg(bHLayout).size();
    }

    const tagBkgImg* img(bool bHLayout, UINT uIdx)
    {
        const tagBkgImg* pRet = NULL;
        _vecImg(bHLayout).get(uIdx, [&](const tagBkgImg& bkgImg){
            pRet = &bkgImg;
        });
        return pRet;
    }
};

class COlBkgDir : public CImgDir
{
public:
    COlBkgDir() = default;

    COlBkgDir(const tagFileInfo& fileInfo, const tagOlBkgList& olBkgList);

private:
    bool m_bDownloading = false;

    tagOlBkgList m_olBkgList;

    vector<wstring> m_vecHFile;
    vector<wstring> m_vecVFile;

private:
    wstring displayName() const override
    {
        return fileName();
    }

    void _onFindFile(TD_PathList&, TD_XFileList&) override {}

    wstring _iconFile() const
    {
        if (m_olBkgList.lstBkg.empty())
        {
            return L"";
        }
        return this->path() + __wcPathSeparator + m_olBkgList.lstBkg.front().strFile;
    }

    bool _genIcon();

    wstring _genSubImg(class CAddBkgView&, XThread&, bool bHLayout, TD_Img&) override;

    bool _downloadSubImg(cwstr strFile, XThread& thread);

public:
    bool downloading() const
    {
        return m_bDownloading;
    }

    void initOlBkg(class CAddBkgView& lv);
};

void zoomoutPixmap(QPixmap& pm, int cx, int cy,  bool bCut);
void zoomoutPixmap(QImage& img, int cx, int cy, bool bCut);
