
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

    if (fileInfo.uFileSize < 50000)
    {
        __yield();
        return NULL;
    }

    __usleep(g_uMsScanYield);

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
            m_vecHFile.push_back(strFile);
        }
        if (m_pmIcon.height() >= __BKG_MaxSide)
        {
            m_vecVFile.push_back(strFile);
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
    __usleep(g_uMsScanYield*3);

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

bool CImgDir::genSubImg(CAddBkgView& lv, XThread& thread)
{
    UINT uGenCount = 12+3*lv.scrollPos(); // +3*ceil(lv.scrollPos());

    auto bHLayout = lv.isHLayout();
    auto prevCount = _vecImg(bHLayout).size();
    if (prevCount >= uGenCount)
    {
        return false;
    }

    auto& bVDownloading = bHLayout?m_bHDownloading:m_bVDownloading;
    TD_Img pm;
    auto uCount = bkgCount(bHLayout);
    cauto vecFile = _vecFile(bHLayout);
    if (uCount < vecFile.size())
    {
        cauto strFile = vecFile[uCount];
        cauto qsFile = __WS2Q(strFile);
        if (!pm.load(qsFile))
        {
            if (!bVDownloading)
            {
                bVDownloading = true;
                __app.sync([&]{
                    lv.update();
                });
            }

            if (!_downloadSubImg(strFile, thread))
            {
                return false;
            }
            (void)pm.load(qsFile);

            if (uCount + 1 == vecFile.size())
            {
                bVDownloading = false;
                __app.sync([&]{
                    lv.update();
                });
            }
        }
    }
    else
    {
        if (bVDownloading)
        {
            bVDownloading = false;
            __app.sync([&]{
                lv.update();
            });
        }

        wstring strFile;
        if (!m_paSubFile.get(m_uPos, [&](XFile& file){
            strFile = file.path();
            (void)_loadBkg(strFile, pm);
        }))
        {
            return false;
        }
        m_uPos++;

        if (bHLayout)
        {
            if (pm.height() >= __BKG_MaxSide)
            {
                m_vecVFile.push_back(strFile);
            }
            if (pm.width() < __BKG_MaxSide)
            {
                return true;
            }
            m_vecHFile.push_back(strFile);
        }
        else
        {
            if (pm.width() >= __BKG_MaxSide)
            {
                m_vecHFile.push_back(strFile);
            }
            if (pm.height() < __BKG_MaxSide)
            {
                return true;
            }
            m_vecVFile.push_back(strFile);
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

#if __android
#define __syncImg __app.syncex //规避安卓切后台问题，理论上会出现死锁。或改为vecImg直接add
#else
#define __syncImg __app.sync //非阻塞，cb必须在100毫秒内
#endif
    __syncImg([&, bHLayout, uZoomoutAll, pm]()mutable{
        if (lv.imgDir() != this)
        {
            return;
        }

        if (lv.isHLayout() != bHLayout)
        {
            return;
        }

        auto& vecImg = _vecImg(bHLayout);
        auto& img = vecImg.add(TD_Img());//vecImg.emplace_back(pm);
        img.swap(pm);
        lv.repaint(); //lv.update();

        if (uZoomoutAll)
        {
            auto itr = vecImg.rbegin();
            for (++itr; itr != vecImg.rend(); ++itr)
            {
                TD_Img&& temp = itr->scaledToWidth(uZoomoutAll, Qt::SmoothTransformation);
                itr->swap(temp);
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
    //fsutil::createDir(strDir);

    for (cauto olBkg : m_olBkgList.lstBkg)
    {
        cauto strFile = strDir + olBkg.strFile;
        //tagFileInfo fi(*this, strFile);
        //auto pSubFile = new XFile(fi);
        //m_paSubFile.add(pSubFile);

        if (olBkg.cx >= olBkg.cy || (int)olBkg.cx >= __BKG_MaxSide)
        {
            _vecFile(true).push_back(strFile);
        }

        if (olBkg.cx <= olBkg.cy || (int)olBkg.cy >= __BKG_MaxSide)
        {
            _vecFile(false).push_back(strFile);
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

void COlBkgDir::initOlBkg(CAddBkgView& lv)
{
    auto strOlBkgDir = g_strWorkDir + __wcPathSeparator + __olBkgDir;
    this->setDir(strOlBkgDir);
    (void)fsutil::createDir(strOlBkgDir);
    strOlBkgDir.push_back(__wcPathSeparator);

    map<wstring, uint64_t> mapFile;
    auto fnCheck = [&](const tagFileInfo& fi) {
        auto itr = mapFile.find(strutil::lowerCase_r(fi.strName));
        return itr != mapFile.end() && itr->second == fi.uFileSize;
    };

    list<COlBkgDir*> lstDir;
    set<wstring> setCatName;
    for (cauto olBkg : __app.getMdlMgr().olBkg())
    {
        setCatName.insert(strutil::lowerCase_r(olBkg.catName));

        mapFile.clear();
        for (cauto olBkg : olBkg.lstBkg)
        {
            mapFile[strutil::lowerCase_r(olBkg.strFile)] = olBkg.uFileSize;
        }

        cauto catDir = strOlBkgDir + olBkg.catName + __wcPathSeparator;
        bool bRet = fsutil::findSubFile(catDir, [&](tagFileInfo& fi) {
            if (!fnCheck(fi))
            {
                fsutil::removeFile(catDir + fi.strName);
            }
        });
        if (!bRet)
        {
            (void)fsutil::createDir(catDir);

#if __android // 安卓背景图片迁移
            cauto strOrgDir = __app.getModel().androidOrgPath(__olBkgDir) + olBkg.catName + L'/';
            fsutil::findSubFile(strOrgDir, [&](tagFileInfo& fi) {
                if (!fnCheck(fi))
                {
                    fsutil::removeFile(strOrgDir + fi.strName);
                }
                else
                {
                    fsutil::moveFile(strOrgDir + fi.strName, catDir + fi.strName);
                }
            });
#endif
        }

        tagFileInfo fileInfo;
        fileInfo.pParent = this;
        fileInfo.strName = olBkg.catName;
        auto pDir = new COlBkgDir(fileInfo, olBkg);

        if (pDir->_genIcon())
        {
            m_paSubDir.add(pDir);
        }
        else
        {
            lstDir.push_back(pDir);
        }
    }

    if (m_paSubDir)
    {
        __app.sync([&]{
            lv.update();
        });
    }

    fsutil::findSubDir(strOlBkgDir, [&](tagFileInfo& fi) {
        if (setCatName.find(strutil::lowerCase_r(fi.strName)) == setCatName.end())
        {
            (void)fsutil::removeDirTree(strOlBkgDir + fi.strName);
        }
    });

    if (lstDir.empty())
    {
        return;
    }

    lv.showLoading(true);
    m_bDownloading = true;

    __app.thread(10, [&, lstDir](XThread& thr)mutable {
        auto pDir = lstDir.front();
        if (pDir->_downloadSubImg(pDir->_iconFile(), thr))
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

//#include "../../../Common2.1/3rd/curl/include/curl/curl.h"

bool COlBkgDir::_downloadSubImg(cwstr strFile, XThread& thread)
{
    cauto strUrl = m_olBkgList.url() + strutil::toAsc(fsutil::GetFileName(strFile));

    int nRet = curlutil::curlDownload(thread, strUrl, strFile, 0, 4);
    if (nRet != 0)
    {
        fsutil::removeFile(strFile);

        g_logger << "downloadSubImg fail: " << nRet << ", url: " >> strUrl;
        if (-404 != nRet) /*if (CURLcode::CURLE_COULDNT_RESOLVE_PROXY == nRet
            || CURLcode::CURLE_COULDNT_RESOLVE_HOST == nRet
            || CURLcode::CURLE_COULDNT_CONNECT == nRet)*/
        {
            g_logger << "dowloadBkg fail: " >> strUrl;
            (void)thread.usleep(5000);
        }
        return false;
    }

    return true;
}
