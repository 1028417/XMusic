
#include "xmusic.h"

#include "imgdir.h"

XThread *g_thrGenSubImg = NULL;
UINT g_uMsScanYield = 1;

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

#define __szSubIngFilter 640

template  <class T=QPixmap>
inline static bool _loadSubImg(cwstr strFile, T& pm)
{
    if (!pm.load(__WS2Q(strFile)))
    {
        return false;
    }

    if (pm.width()<__szSubIngFilter || pm.height()<__szSubIngFilter)
    {
        return false;
    }

    return true;
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

    QPixmap&& pm = m_pmIcon.width() < m_pmIcon.height()
            ? m_pmIcon.scaledToWidth(__szSnapshot, Qt::SmoothTransformation)
            : m_pmIcon.scaledToHeight(__szSnapshot, Qt::SmoothTransformation);
    m_pmIcon.swap(pm);
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

void CImgDir::genSubImgs(CAddBkgView& lv, bool bHLayout)
{
    auto& uPos = bHLayout?m_uHPos:m_uVPos;
    if (uPos >= m_paSubFile.size())
    {
        return;
    }

    if (m_vecHImgs.capacity() == 0)
    {
        m_vecHImgs.reserve(m_paSubFile.size());
        m_vecVImgs.reserve(m_paSubFile.size());
    }

    if (g_thrGenSubImg)
    {
        g_thrGenSubImg->cancel();
    }
    else
    {
        g_thrGenSubImg = &__app.thread(); //new XThread;
    }
    g_thrGenSubImg->start([&, bHLayout]{
        do {
            if (!_genSubImgs(lv, bHLayout))
            {
                return;
            }

        } while (g_thrGenSubImg->usleep(30));
    });
}

bool CImgDir::_genSubImgs(CAddBkgView& lv, bool bHLayout)
{
    wstring strFile;
    if (!m_paSubFile.get(m_uHPos, [&](XFile& file){
        strFile = file.path();
    }))
    {
        return false;
    }

    m_uHPos++;
    m_uVPos++;
    TD_Img pm;

#if __windows || __mac
    if (m_paSubFile.get(m_uHPos, [&](XFile& file){
        cauto strFile2 = file.path();
        m_uHPos++;
        m_uVPos++;
        TD_Img pm2;
        mtutil::concurrence([&]{
            (void)_loadSubImg(strFile, pm);
        }, [&]{
            (void)_loadSubImg(strFile2, pm2);
        });

        if (!pm.isNull() || !pm2.isNull())
        {
            __app.sync([&, strFile, pm, strFile2, pm2]()mutable{
                if (this != lv.imgDir())
                {
                     m_uHPos-=2;
                     m_uVPos-=2;
                     return;
                }

                if (!pm.isNull())
                {
                    _genSubImgs(strFile, pm);
                }

                if (!pm2.isNull())
                {
                    _genSubImgs(strFile2, pm2);
                }

                lv.update();
            });
        }
    }))
    {
        return true;
    }
#endif

    if (_loadSubImg(strFile, pm))
    {
        __app.sync([&, strFile, pm]()mutable{
            if (this != lv.imgDir())
            {
                m_uHPos--;
                m_uVPos--;
                return;
            }

            _genSubImgs(strFile, pm);

            lv.update();
        });
    }

    return true;
}

void CImgDir::_genSubImgs(cwstr strFile, TD_Img& pm)
{
    auto& vecImgs = pm.width()>pm.height()?m_vecHImgs:m_vecVImgs;
    auto prevCount = vecImgs.size();
    int szZoomout = g_screen.nMaxSide;
    if (prevCount >= 4)
    {
        szZoomout /= 3;

        auto n = prevCount/18;
        if (n > 1)
        {
            szZoomout /= pow(n, 0.3);
            if (prevCount % 18 == 0)
            {
                for (auto& bkgImg : vecImgs)
                {
                    zoomoutPixmap(bkgImg.pm, szZoomout, szZoomout, false);
                }
            }
        }
        else if (4 == prevCount)
        {
            for (auto& bkgImg : vecImgs)
            {
                zoomoutPixmap(bkgImg.pm, szZoomout, szZoomout, false);
            }
        }
    }
    else
    {
        szZoomout /= 2;
    }

    zoomoutPixmap(pm, szZoomout, szZoomout, false);

    vecImgs.emplace_back(pm, strFile);
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

bool COlBkgDir::_genSubImgs(CAddBkgView& lv, bool bHLayout)
{
    auto& uPos = bHLayout?m_uHPos:m_uVPos;
    wstring strFile;
    if (!m_paSubFile.get(uPos, [&](XFile& file){
        strFile = file.path();
    }))
    {
        return false;
    }

    if (!fsutil::existFile(strFile))
    {
        return false;
    }

    uPos++;
    TD_Img pm;

    if (_loadSubImg(strFile, pm))
    {
        __app.sync([&, strFile, pm]()mutable{
            if (this != lv.imgDir())
            {
                uPos--;
                return;
            }

            CImgDir::_genSubImgs(strFile, pm);

            lv.update();
        });
    }

    return true;
}

template <class T=QPixmap>
inline static void _zoomoutPixmap(T& pm, int cx, int cy, bool bCut)
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

void zoomoutPixmap(QPixmap& pm, int cx, int cy, bool bCut)
{
    _zoomoutPixmap(pm, cx, cy, bCut);
}

void zoomoutPixmap(QImage& img, int cx, int cy, bool bCut)
{
    _zoomoutPixmap(img, cx, cy, bCut);
}
