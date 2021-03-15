
#include "xmusic.h"

#include "imgdir.h"

UINT g_uMsScanYield = 1;

template <class T=QPixmap>
inline static void _cutPixmap(T& pm, float fHWRate)
{
    auto cx = pm.width();
    auto cy = pm.height();

    auto cyMax = cx * fHWRate;
    if (cy - cyMax > cyMax/10)
    {
        T&& temp = pm.copy(0, (cy-cyMax)/2, cx, cyMax);
        pm.swap(temp);
        return;
    }

    auto cxMax = cy / fHWRate;
    if (cx - cxMax > cxMax/10)
    {
        T&& temp = pm.copy((cx-cxMax)/2, 0, cxMax, cy);
        pm.swap(temp);
    }
}

template <class T=QPixmap>
inline static void _zoomoutPixmap(T& pm, int cx, int cy, bool bCut)
{
    if (bCut)
    {
        _cutPixmap(pm, (float)cy/cx);
    }

    auto cxPm = pm.width();
    if (0 == cxPm)
    {
        return;
    }
    auto cyPm = pm.height();

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

void zoomoutPixmap(QPixmap& pm, int cx, int cy, bool bCut)
{
    _zoomoutPixmap(pm, cx, cy, bCut);
}

void zoomoutPixmap(QImage& img, int cx, int cy, bool bCut)
{
    _zoomoutPixmap(img, cx, cy, bCut);
}

wstring CImgDir::displayName() const
{
#if __android
    if (m_fi.pParent)
    {
        if (m_fi.pParent->parent())
        {
            return ((CImgDir*)m_fi.pParent)->displayName() + L'/' + m_fi.strName;
        }
        else
        {
            return L'/' + m_fi.strName;
        }
    }
    else
    {
        return L"根目录";
    }

#else
    return path();
#endif
}

#define __BKG_MaxSide (g_screen.nMaxSide*4/5) //1800
#define __BKG_MinSide (g_screen.nMinSide*4/5) //800

template  <class T=QPixmap>
inline static bool _loadBkg(cwstr strFile, T& pm)
{
    if (!pm.load(__WS2Q(strFile)))
    {
        return false;
    }

    if (pm.width()>=__BKG_MaxSide && pm.height()>=__BKG_MinSide)
    {
        return true;
    }
    if (pm.width()>=__BKG_MinSide && pm.height()>=__BKG_MaxSide)
    {
        return true;
    }

    return false;
}

static const SSet<wstring>& g_setImgExtName = SSet<wstring>(L"jpg", L"jpe", L"jpeg", L"jfif", L"png", L"bmp");

#define __szSnapshot 160

inline bool CImgDir::_genIcon(cwstr strFile)
{
    if (!_loadBkg(strFile, m_pmIcon))
    {
        m_pmIcon = QPixmap();
        return false;
    }

    if (m_pmIcon.width() >= __BKG_MaxSide)
    {
        m_lstHPos.push_back(strFile);
    }
    if (m_pmIcon.height() >= __BKG_MaxSide)
    {
        m_lstVPos.push_back(strFile);
    }
    m_uPos = 1;

    /*QPixmap&& pm = m_pmIcon.width() < m_pmIcon.height()
            ? m_pmIcon.scaledToWidth(__szSnapshot, Qt::SmoothTransformation)
            : m_pmIcon.scaledToHeight(__szSnapshot, Qt::SmoothTransformation);
    m_pmIcon.swap(pm);
    _cutPixmap(m_pmIcon, 1.0f);*/

    _zoomoutPixmap(m_pmIcon, __szSnapshot, __szSnapshot, true);
    return true;
}

XFile* CImgDir::_newSubFile(const tagFileInfo& fileInfo)
{
    if (!m_bRunSignal)
    {
        return NULL;
    }
    if (!usleepex(g_uMsScanYield))
    {
        return NULL;
    }

    cauto strExtName = strutil::lowerCase_r(fsutil::GetFileExtName(fileInfo.strName));
    if (!g_setImgExtName.includes(strExtName))
    {
        return NULL;
    }

    if (m_pmIcon.isNull())//if (m_paSubFile.empty())
    {
        cauto strFile = path()+__wcPathSeparator+fileInfo.strName;
        if (!_genIcon(strFile))
        {
            return NULL;
        }
    }

    return new XFile(fileInfo);
}

CPath* CImgDir::_newSubDir(const tagFileInfo& fileInfo)
{
    if (!m_bRunSignal)
    {
        return NULL;
    }
    if (!usleepex(g_uMsScanYield*3))
    {
        return NULL;
    }

    if (fileInfo.strName.front() == L'.')
    {
        return NULL;
    }

    if (__pkgName == fileInfo.strName)
    {
        return NULL;
    }
    if (NULL == m_fi.pParent && L"Android" == fileInfo.strName) // TODO 改为并发扫
    {
        return NULL;
    }

    return new CImgDir(m_bRunSignal, fileInfo);
}

const TD_Img* CImgDir::img(bool bHLayout, UINT uIdx) const
{
    auto& vecImgs = bHLayout?m_vecHImgs:m_vecVImgs;
    if (uIdx < vecImgs.size())
    {
        return &vecImgs[uIdx].pm;
    }
    return NULL;
}

wstring CImgDir::imgPath(bool bHLayout, UINT uIdx) const
{
    auto& vecImgs = bHLayout?m_vecHImgs:m_vecVImgs;
    if (uIdx < vecImgs.size())
    {
        return vecImgs[uIdx].strPath;
    }
    return L"";
}

bool CImgDir::genSubImg(CAddBkgView& lv, UINT uGenCount, XThread& thread)
{
    auto bHLayout = lv.isHLayout();
    auto& vecImgs = bHLayout?m_vecHImgs:m_vecVImgs;
    auto prevCount = vecImgs.size();
    if (prevCount >= uGenCount)
    {
        return false;
    }

    wstring strFile;
    TD_Img pm;

    auto& lstPos = bHLayout?m_lstHPos:m_lstVPos;
    if (!lstPos.empty())
    {
        strFile = lstPos.front();
        lstPos.pop_front();
        if (!_loadBkg(strFile, pm))
        {
            return true;
        }
    }
    else
    {
        bool bRet = false;
        m_paSubFile.get(m_uPos, [&](XFile& file){
            strFile = file.path();
            bRet = _loadSubImg(file, pm, thread);
        });
        if (!bRet)
        {
            return false;
        }
        m_uPos++;

        if (bHLayout)
        {
            if (pm.height() >= __BKG_MaxSide)
            {
                m_lstVPos.push_back(strFile);
            }
            if (pm.width() < __BKG_MaxSide)
            {
                return true;
            }
        }
        else
        {
            if (pm.width() >= __BKG_MaxSide)
            {
                m_lstHPos.push_back(strFile);
            }
            if (pm.height() < __BKG_MaxSide)
            {
                return true;
            }
        }
    }

    int cx = g_screen.nMaxSide;
    int cy = g_screen.nMinSide;
    if (!bHLayout)
    {
        std::swap(cx, cy);
    }

    float fZoomoutRate = 2.2f;
    UINT uZoomoutAll = 0;

    if (prevCount >= 4)
    {
        fZoomoutRate = 3.3f;

        auto n = prevCount/18;
        if (n > 1)
        {
            fZoomoutRate += pow(n, 0.3);
            if (prevCount % 18 == 0)
            {
                uZoomoutAll = cx/fZoomoutRate;
            }
        }
        else if (4 == prevCount)
        {
            uZoomoutAll = cx/fZoomoutRate;
        }
    }

    _zoomoutPixmap(pm, cx/fZoomoutRate, cy/fZoomoutRate, true);

    if (!thread)
    {
        return false;
    }

    __app.sync([&, bHLayout, uZoomoutAll, pm, strFile]()mutable{
        if (lv.imgDir() != this)
        {
            return;
        }

        if (lv.isHLayout() != bHLayout)
        {
            auto& lstPos = bHLayout?m_lstHPos:m_lstVPos;
            lstPos.push_front(strFile);
            return;
        }

        auto& vecImgs = bHLayout?m_vecHImgs:m_vecVImgs;
        if (vecImgs.capacity() == 0)
        {
            vecImgs.reserve(m_paSubFile.size());
        }
        else
        {
            if (uZoomoutAll)
            {
                for (auto& bkgImg : vecImgs)
                {
                    decltype(bkgImg.pm)&& temp = bkgImg.pm.scaledToWidth(uZoomoutAll, Qt::SmoothTransformation);
                    bkgImg.pm.swap(temp);
                }
            }
        }

        vecImgs.emplace_back(pm, strFile);

        lv.update();
    });

    return true;
}

bool CImgDir::_loadSubImg(XFile& file, TD_Img& pm, XThread&)
{
    (void)_loadBkg(file.path(), pm);
    return true;
}

COlBkgDir::COlBkgDir(const tagFileInfo& fileInfo, const tagOlBkg& olBkg)
    : CImgDir(m_bRunsignal, fileInfo)
    , m_olBkg(olBkg)
{
    fsutil::createDir(path());

    for (cauto strFile : m_olBkg.lstFiles)
    {
        tagFileInfo fi(*this, strutil::fromAsc(strFile));
        auto pSubFile = new XFile(fi);
        m_paSubFile.add(pSubFile);
    }
}

string COlBkgDir::url(XFile& file)
{
    return "https://" + m_olBkg.accName + "-generic.pkg.coding.net/"
            + m_olBkg.prjName + "/" + m_olBkg.artiName + "/" + strutil::toAsc(file.fileName());
}

#include "../../../Common2.1/3rd/curl/include/curl/curl.h"

void COlBkgDir::initOlBkg(CAddBkgView& lv)
{
    static bool s_bFlag = false;
    if (s_bFlag)
    {
        return;
    }
    s_bFlag = true;

    list<XFile*> lstFiles;

    for (cauto olBkg : __app.getModel().olBkg())
    {
        tagFileInfo fileInfo;
        fileInfo.pParent = this;
        fileInfo.strName = olBkg.catName;
        auto pDir = new COlBkgDir(fileInfo, olBkg);

        for (auto pFile : pDir->files())
        {
            cauto strFile = pFile->path();
            if (!fsutil::existFile(strFile))
            {
                lstFiles.push_back(pFile);
            }
            else
            {
                (void)pDir->_genIcon(strFile);
                m_paSubDir.add(pDir);
            }
            break;
        }
    }

    if (lstFiles.empty())
    {
        return;
    }

    auto thread = &__app.thread();
    thread->start(10, [&, thread, lstFiles]()mutable {
        auto pFile = lstFiles.front();
        if (_downloadSubImg(*pFile, *thread))
        {
            auto pDir = (COlBkgDir*)pFile->parent();
            pDir->_genIcon(pFile->path());
            __app.sync([&, pDir]{
                m_paSubDir.add(pDir);
                lv.update();
            });

            lstFiles.pop_front();
            if (lstFiles.empty())
            {
                return false;
            }
        }
        return true;
    });
}

bool COlBkgDir::_downloadSubImg(XFile& file, XThread& thread)
{
    auto pDir = (COlBkgDir*)file.parent();
    cauto strFile = file.path();
    cauto strUrl = pDir->url(file);

    tagCurlOpt curlOpt(true);
    CDownloader downloader(curlOpt);
    CByteBuffer bbfBkg;
    int nRet = downloader.syncDownload(thread, strUrl, bbfBkg);
    if (nRet != 0)
    {
        if (CURLcode::CURLE_COULDNT_RESOLVE_PROXY == nRet
            || CURLcode::CURLE_COULDNT_RESOLVE_HOST == nRet
            || CURLcode::CURLE_COULDNT_CONNECT == nRet)
        {
            (void)thread.usleep(5000);
        }
        return false;
    }

    if (!OFStream::writefilex(strFile, true, bbfBkg))
    {
        return false;
    }

    return true;
}

bool COlBkgDir::_loadSubImg(XFile& file, TD_Img& pm, XThread& thread)
{
    cauto strFile = file.path();
    if (!fsutil::existFile(strFile))
    {
        if (!_downloadSubImg(file, thread))
        {
            return false;
        }
    }

    (void)_loadBkg(strFile, pm);
    return true;
}
