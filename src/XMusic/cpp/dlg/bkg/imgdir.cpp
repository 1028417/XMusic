
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

#define __BKG_MIN 1800

template  <class T=QPixmap>
inline static bool _loadSubImg(cwstr strFile, T& pm)
{
    if (!pm.load(__WS2Q(strFile)))
    {
        return false;
    }

    if (pm.width()>=__BKG_MIN && pm.height()>=800)
    {
        return true;
    }
    if (pm.width()>=800 && pm.height()>=__BKG_MIN)
    {
        return true;
    }

    return false;
}

static const SSet<wstring>& g_setImgExtName = SSet<wstring>(L"jpg", L"jpe", L"jpeg", L"jfif", L"png", L"bmp");

#define __szSnapshot 160

inline bool CImgDir::_genIcon(cwstr strFile)
{
    if (!_loadSubImg(strFile, m_pmIcon))
    {
        m_pmIcon = QPixmap();
        return false;
    }

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

bool CImgDir::genSubImg(CAddBkgView& lv, UINT uGenCount)
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

    cauto vecPos = bHLayout?m_vecHPos:m_vecVPos;
    if (vecPos.size() > prevCount)
    {
        if (!m_paSubFile.get(vecPos[prevCount], [&](XFile& file){
            strFile = file.path();
        }))
        {
            return false;
        }
        if (!_loadSubImg(strFile, pm))
        {
            return true;
        }
    }
    else
    {
        if (!m_paSubFile.get(m_uPos, [&](XFile& file){
            strFile = file.path();
        }))
        {
            return false;
        }
        m_uPos++;

        if (!_loadSubImg(strFile, pm))
        {
            return true;
        }
        if (bHLayout)
        {
            if (pm.height() >= __BKG_MIN)
            {
                m_vecVPos.push_back(m_uPos-1);
            }
            if (pm.width() < __BKG_MIN)
            {
                return true;
            }
            m_vecHPos.push_back(m_uPos-1);
        }
        else
        {
            if (pm.width() >= __BKG_MIN)
            {
                m_vecHPos.push_back(m_uPos-1);
            }
            if (pm.height() < __BKG_MIN)
            {
                return true;
            }
            m_vecVPos.push_back(m_uPos-1);
        }
    }

    int cx = g_screen.nMaxSide;
    int cy = g_screen.nMinSide;
    if (!bHLayout)
    {
        std::swap(cx, cy);
    }

    float fZoomoutRate = 2.0f;
    UINT uZoomoutAll = 0;

    if (prevCount >= 4)
    {
        fZoomoutRate = 3;

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

    __app.sync([&, bHLayout, uZoomoutAll, pm, strFile]()mutable{
        if (lv.imgDir() != this || lv.isHLayout() != bHLayout)
        {
            m_uPos--;
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

const tagOlBkg COlBkgDir::s_olBkg;

COlBkgDir::COlBkgDir(signal_t bRunSignal, const tagFileInfo& fileInfo, const tagOlBkg& olBkg)
    : CImgDir(bRunSignal, fileInfo)
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

void COlBkgDir::tryAdd(COlBkgDir& dir, CAddBkgView& lv)
{
    if (!dir.icon().isNull())
    {
        return;
    }

    dir.files().front([&](XFile& file){
        if (dir._genIcon(file.path()))
        {
            __app.sync([&]{
                m_paSubDir.add(dir);
                lv.update();
            });
        }
    });
}
