
#include "xmusic.h"

#include "imgdir.h"

XThread *g_thrGenSubImg = NULL;
UINT g_uMsScanYield = 1;

class COlImgDir : public CImgDir
{
public:
    COlImgDir(signal_t bRunSignal)
        : CImgDir(bRunSignal)
    {
    }

    COlImgDir(signal_t bRunSignal, const tagFileInfo& fileInfo)
        : CImgDir(bRunSignal, fileInfo)
    {
    }

};

/*void CImgDir::_onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile)
{
//#if __android
//    if (__sdcardDir == m_fi.strName)
//    {
//        auto strRoot = L"/storage/";
//        (void)fsutil::findSubDir(strRoot, [&](cwstr strSubDir) {
//            paSubDir.addFront(new CImgDir(m_bRunSignal, strRoot + strSubDir));
//        });
//    }
//#endif

    if (NULL == m_fi.pParent)
    {
        auto pOlImgDir = new COlImgDir(m_bRunSignal);
        cauto strDir = g_strWorkDir + L"/bkg_ol/";
        pOlImgDir->setDir(strDir);
        paSubDir.add(pOlImgDir);
    }

    CPath::_onFindFile(paSubDir, paSubFile);
}*/

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

    if (m_paSubFile.empty())
    {
        if (!_loadSubImg(XFile(fileInfo).path(), m_pmIcon))
        {
            if (!m_bRunSignal)
            {
                return NULL;
            }
            if (!usleepex(g_uMsScanYield))
            {
                return NULL;
            }

            m_pmIcon = QPixmap();
            return NULL;
        }

        QPixmap&& pm = m_pmIcon.width() < m_pmIcon.height()
                ? m_pmIcon.scaledToWidth(__szSnapshot, Qt::SmoothTransformation)
                : m_pmIcon.scaledToHeight(__szSnapshot, Qt::SmoothTransformation);
        m_pmIcon.swap(pm);
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

const TD_Img* CImgDir::img(UINT uIdx) const
{
    if (uIdx < m_vecImgs.size())
    {
        return &m_vecImgs[uIdx].pm;
    }
    return NULL;
}

wstring CImgDir::imgPath(UINT uIdx) const
{
    if (uIdx < m_vecImgs.size())
    {
        return m_vecImgs[uIdx].strPath;
    }
    return L"";
}

void CImgDir::genSubImgs(CAddBkgView& lv)
{
    if (m_uPos >= m_paSubFile.size())
    {
        return;
    }

    if (m_vecImgs.capacity() == 0)
    {
        m_vecImgs.reserve(m_paSubFile.size());
    }

    if (g_thrGenSubImg)
    {
        g_thrGenSubImg->cancel();
    }
    else
    {
        g_thrGenSubImg = &__app.thread(); //new XThread;
    }
    g_thrGenSubImg->start([&]{
        do {
            if (!_genSubImgs(lv))
            {
                return;
            }

        } while (g_thrGenSubImg->usleep(30));
    });
}

bool CImgDir::_genSubImgs(CAddBkgView& lv)
{
    wstring strFile;
    if (!m_paSubFile.get(m_uPos, [&](XFile& file){
        strFile = file.path();
    }))
    {
        return false;
    }

    m_uPos++;

    TD_Img pm;

#if __windows || __mac
    if (m_paSubFile.get(m_uPos, [&](XFile& file){
        m_uPos++;

        cauto strFile2 = file.path();
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
                     m_uPos-=2;
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
                m_uPos--;
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
    auto prevCount = m_vecImgs.size();
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
                for (auto& bkgImg : m_vecImgs)
                {
                    zoomoutPixmap(bkgImg.pm, szZoomout, szZoomout, true);
                }
            }
        }
        else if (4 == prevCount)
        {
            for (auto& bkgImg : m_vecImgs)
            {
                zoomoutPixmap(bkgImg.pm, szZoomout, szZoomout, true);
            }
        }
    }
    else
    {
        szZoomout /= 2;
    }

    zoomoutPixmap(pm, szZoomout, szZoomout, true);

    m_vecImgs.emplace_back(pm, strFile);
}
