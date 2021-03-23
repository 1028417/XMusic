
#pragma once

#include "model/model.h"

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

    QPixmap m_pmIcon;

    UINT m_uPos = 0;

    list<wstring> m_lstHPos;
    list<wstring> m_lstVPos;

    vector<tagBkgImg> m_vecHImgs;
    vector<tagBkgImg> m_vecVImgs;

protected:
    bool _genIcon(cwstr strFile);

    virtual bool _loadSubImg(XThread&, XFile&, TD_Img&);

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
        m_lstHPos.clear();
        m_lstVPos.clear();
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

    bool genSubImg(class CAddBkgView&, XThread&);
};

class COlBkgDir : public CImgDir
{
public:
    COlBkgDir() = default;

    COlBkgDir(const tagFileInfo& fileInfo, const tagOlBkg& olBkg);

private:
    bool m_bDownloading = false;

    tagOlBkg m_olBkg;

private:
    wstring displayName() const override
    {
        return fileName();
    }

    void _onFindFile(TD_PathList&, TD_XFileList&) override {}

    bool _downloadSubImg(XFile& file, XThread& thread);
    bool _loadSubImg(XThread&, XFile&, TD_Img&) override;

public:
    bool downloading() const
    {
        return m_bDownloading;
    }

    void initOlBkg(class CAddBkgView& lv);

    string url(XFile& file);
};

void zoomoutPixmap(QPixmap& pm, int cx, int cy,  bool bCut);
void zoomoutPixmap(QImage& img, int cx, int cy, bool bCut);
