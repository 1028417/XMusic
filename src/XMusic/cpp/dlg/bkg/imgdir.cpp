
#include "xmusic.h"

#include "imgdir.h"

#include <QMovie>

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
        if (m_pmIcon.width() >= __BKG_MaxSide)
        {
            m_lstHFile.push_back(strFile);
        }
        if (m_pmIcon.height() >= __BKG_MaxSide)
        {
            m_lstVFile.push_back(strFile);
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

wstring CImgDir::_genSubImg(CAddBkgView&, XThread&, bool bHLayout, TD_Img& pm)
{
    wstring strFile;
    auto& lstFile = bHLayout?m_lstHFile:m_lstVFile;
    if (!lstFile.empty())
    {
        strFile = lstFile.front();
        lstFile.pop_front();
        (void)_loadBkg(strFile, pm);
    }
    else
    {
        if (!m_paSubFile.get(m_uPos, [&](XFile& file){
            strFile = file.path();
            (void)_loadBkg(strFile, pm);
        }))
        {
            return L"";
        }
        m_uPos++;

        if (bHLayout)
        {
            if (pm.height() >= __BKG_MaxSide)
            {
                m_lstVFile.push_back(strFile);
            }
            if (pm.width() < __BKG_MaxSide)
            {
                return L"";
            }
        }
        else
        {
            if (pm.width() >= __BKG_MaxSide)
            {
                m_lstHFile.push_back(strFile);
            }
            if (pm.height() < __BKG_MaxSide)
            {
                return L"";
            }
        }
    }

    /*auto& vecImg = _vecImg(bHLayout);
    if (vecImg.capacity() == 0)
    {
        vecImg.reserve(m_paSubFile.size());
    }*/

    return strFile;
}

bool CImgDir::genSubImg(CAddBkgView& lv, XThread& thread)
{
    UINT uGenCount = 12+3*lv.scrollPos(); // +3*ceil(lv.scrollPos());

    auto bHLayout = lv.isHLayout();
    auto prevCount = _vecImg(bHLayout).size();
    if (prevCount >= uGenCount)
    {
        return false;
    }

    TD_Img pm;
    cauto strFile = _genSubImg(lv, thread, bHLayout, pm);
    if (strFile.empty())
    {
        return false;
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
            //auto& lstFile = bHLayout?m_lstHFile:m_lstVFile;
            //lstFile.push_front(strFile);
            return;
        }

        auto& vecImg = _vecImg(bHLayout);
        auto& bkgImg = vecImg.add(tagBkgImg());//vecImg.emplace_back(pm, strFile);
        bkgImg.pm.swap(pm);
        bkgImg.strPath = strFile;
        lv.repaint(); //lv.update();

        if (uZoomoutAll)
        {
            auto itr = vecImg.rbegin();
            for (++itr; itr != vecImg.rend(); ++itr)
            {
                TD_Img&& temp = itr->pm.scaledToWidth(uZoomoutAll, Qt::SmoothTransformation);
                itr->pm.swap(temp);
            }
        }
    });

    return true;
}


COlBkgDir::COlBkgDir(const tagFileInfo& fileInfo, const tagOlBkgList& olBkgList)
    : CImgDir(fileInfo)
    , m_olBkgList(olBkgList)
{
    cauto strDir = this->path() + __wcPathSeparator;
    fsutil::createDir(strDir);

    for (cauto olBkg : m_olBkgList.lstBkg)
    {
        cauto strFile = strDir + olBkg.strFile;
        //tagFileInfo fi(*this, strFile);
        //auto pSubFile = new XFile(fi);
        //m_paSubFile.add(pSubFile);

        if (olBkg.cx >= olBkg.cy || (int)olBkg.cx >= __BKG_MaxSide)
        {
            m_vecHFile.push_back(strFile);
        }

        if (olBkg.cx <= olBkg.cy || (int)olBkg.cy >= __BKG_MaxSide)
        {
            m_vecVFile.push_back(strFile);
        }
    }

    //_vecImg(true).reserve(m_vecHFile.size());
    //_vecImg(false).reserve(m_vecVFile.size());
}

bool COlBkgDir::_genIcon()
{
    if (!m_pmIcon.load(__WS2Q(_iconFile())))
    {
        return false;
    }

    _zoomoutPixmap(m_pmIcon, __szSnapshot, __szSnapshot, true);
    return true;
}

#include "../../../Common2.1/3rd/curl/include/curl/curl.h"

void COlBkgDir::initOlBkg(CAddBkgView& lv)
{
    list<COlBkgDir*> lstDir;

    for (cauto olBkg : __app.getModel().olBkg())
    {
        tagFileInfo fileInfo;
        fileInfo.pParent = this;
        fileInfo.strName = olBkg.catName;
        auto pDir = new COlBkgDir(fileInfo, olBkg);

        if (pDir->_genIcon())
        {
            _zoomoutPixmap(m_pmIcon, __szSnapshot, __szSnapshot, true);
            m_paSubDir.add(pDir);
        }
        else
        {
            lstDir.push_back(pDir);
        }
    }

    if (lstDir.empty())
    {
        return;
    }

    lv.showLoading(true);
    m_bDownloading = true;

    auto thread = &__app.thread();
    thread->start(10, [&, thread, lstDir]()mutable {
        auto pDir = lstDir.front();
        if (pDir->_downloadSubImg(pDir->_iconFile(), *thread))
        {
            pDir->_genIcon();
            __app.sync([&, pDir]{
                m_paSubDir.add(pDir);
                lv.update();
            });

            lstDir.pop_front();
            if (lstDir.empty())
            {
                if (lv.imgDir() == this)
                {
                    __app.sync([&]{
                        if (lv.imgDir() == this)
                        {
                            lv.showLoading(false);
                        }
                    });
                }
                m_bDownloading = false;
                return false;
            }
        }
        return true;
    });
}

bool COlBkgDir::_downloadSubImg(cwstr strFile, XThread& thread)
{
    cauto strUrl = "https://" + m_olBkgList.accName + "-generic.pkg.coding.net/"
            + m_olBkgList.prjName + "/" + m_olBkgList.artiName + "/"
            + strutil::toAsc(fsutil::GetFileName(strFile));

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
        g_logger << "dowloadBkg fail: " >> strUrl;
        return false;
    }

    if (!OFStream::writefilex(strFile, true, bbfBkg))
    {
        return false;
    }

    return true;
}

wstring COlBkgDir::_genSubImg(CAddBkgView&, XThread& thread, bool bHLayout, TD_Img& pm)
{
    auto uCount = _vecImg(bHLayout).size();
    auto& vecFile = bHLayout?m_vecHFile:m_vecVFile;
    if (uCount >= vecFile.size())
    {
        return L"";
    }

    thread.usleep(100);

    cauto strFile = vecFile[uCount];
    if (!fsutil::existFile(strFile))
    {
//        __app.sync([&]{
//            lv.update();
//        });

        if (!_downloadSubImg(strFile, thread))
        {
            return L"";
        }

//        __app.sync([&]{
//            lv.update();
//        });
    }

    (void)_loadBkg(strFile, pm);
    return strFile;
}
