
#pragma once

#include "model/model.h"

//using TD_Img = QImage;
using TD_Img = QPixmap;

class CImgDir : public CPath//, public CImgDir
{
public:
    CImgDir(signal_t bRunSignal)
        : m_bRunSignal(bRunSignal)
    {
    }

    CImgDir(signal_t bRunSignal, cwstr strDir, CImgDir *pParent)
        : CPath(strDir, pParent)
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

    vector<wstring> m_vecHFile;
    vector<wstring> m_vecVFile;

    SArray<TD_Img> m_vecHImg;
    SArray<TD_Img> m_vecVImg;

    bool m_bHDownloading = false;
    bool m_bVDownloading = false;

protected:
    QPixmap m_pmIcon;

protected:
    bool _genIcon(cwstr strFile);

    inline vector<wstring>& _vecFile(bool bHLayout)
    {
        return bHLayout?m_vecHFile:m_vecVFile;
    }
    inline const vector<wstring>& _vecFile(bool bHLayout) const
    {
        return bHLayout?m_vecHFile:m_vecVFile;
    }

    inline SArray<TD_Img>& _vecImg(bool bHLayout)
    {
        return bHLayout?m_vecHImg:m_vecVImg;
    }
    inline const SArray<TD_Img>& _vecImg(bool bHLayout) const
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

    void _genSubDir(const QFileInfo& fi);
    void _genSubFile(const QFileInfo& fi);

    virtual bool _downloadSubImg(cwstr, XThread&) {return false;}

public:
    void cleanup()
    {
        m_uPos = 0;
        //m_lstHFile.clear();
        //m_lstVFile.clear();
        m_vecHImg.clear();
        m_vecVImg.clear();
    }

    void scanDir(signal_t bRunSignal, const function<void(CImgDir& imgDir)>& cb);

    virtual wstring displayName() const;

    cqpm icon() const
    {
        return m_pmIcon;
    }

    bool genSubImg(class CAddBkgView&, XThread&);

    size_t bkgCount(bool bHLayout) const
    {
        return _vecImg(bHLayout).size();
    }

    const TD_Img* bkgImg(bool bHLayout, UINT uIdx)
    {
        const TD_Img *pImg = NULL;
        _vecImg(bHLayout).get(uIdx, [&](const TD_Img& img){
            pImg = &img;
        });
        return pImg;
    }

    wstring bkgFile(bool bHLayout, UINT uIdx)
    {
        cauto vecFile = _vecFile(bHLayout);
        if (uIdx >= vecFile.size())
        {
            return L"";
        }
        return vecFile[uIdx];
    }

    bool downloading(bool bHLayout) const
    {
        return bHLayout?m_bHDownloading:m_bVDownloading;
    }
};

#define __olBkgDir L"XMusic图库"

class COlBkgDir : public CImgDir
{
public:
    COlBkgDir() = default;

    COlBkgDir(const tagFileInfo& fileInfo, const tagOlBkgList& olBkgList);

private:
    bool m_bDownloading = false;

    tagOlBkgList m_olBkgList;

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

    bool _downloadSubImg(cwstr strFile, XThread& thread) override;

public:
    bool downloading() const
    {
        return m_bDownloading;
    }

    void initOlBkg(class CAddBkgView& lv);

    size_t bkgMaxCount(bool bHLayout) const
    {
        return _vecFile(bHLayout).size();
    }
};

void zoomoutPixmap(QPixmap& pm, int cx, int cy,  bool bCut);
void zoomoutPixmap(QImage& img, int cx, int cy, bool bCut);
