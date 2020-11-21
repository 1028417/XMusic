
#pragma once

#include "dialog.h"

#include "MedialibView.h"

#include "wholeTrackDlg.h"

#include "singerimgdlg.h"

#if __android
#define __OuterDir __sdcardDir
#elif __windows
#define __OuterDir L""
#else
#define __OuterDir fsutil::getHomeDir().toStdWString()
#endif

class COuterDir : public CMediaDir
{
public:
    COuterDir() : CMediaDir(__OuterDir)
    {
    }

    COuterDir(cwstr strPath, CMediaDir *pParent = NULL)
        : CMediaDir(strPath, pParent)
    {
    }

private:
    wstring path() const override
    {
        return m_fi.strName;
    }

    wstring GetPath() const override // 所有平台都需要
    {
        return m_fi.strName;
    }

#if __windows
    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override
    {
        if (NULL == m_fi.pParent)
        {
            std::list<std::wstring> lstDrivers;
            winfsutil::getSysDrivers(lstDrivers);
            for (cauto strDriver : lstDrivers)
            {
                paSubDir.add(new COuterDir(strDriver, this));
            }
        }
        else
        {
            CMediaDir::_onFindFile(paSubDir, paSubFile);
        }
    }

#else
#if __android
    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override
    {
        CMediaDir::_onFindFile(paSubDir, paSubFile);

        if (NULL == m_fi.pParent)
        {
            (void)fsutil::findSubDir(L"/storage", [&](cwstr strSubDir) {
                paSubDir.addFront(new COuterDir(L"/storage/" + strSubDir, this));
            });
        }
    }
#endif

    CMediaRes* subPath(cwstr strSubPath, bool bDir) override
    {
        cauto strOppPath = fsutil::GetOppPath(m_fi.strName, strSubPath);
        if (strOppPath.empty())
        {
#if __android
            if (NULL == m_fi.pParent)
            {
                for (auto pDir : dirs())
                {
                    auto pOuterDir = dynamic_cast<COuterDir*>(pDir);
                    if (NULL == pOuterDir)
                    {
                        break;
                    }

                    auto pMediaRes = pOuterDir->subPath(strSubPath, bDir);
                    if (pMediaRes)
                    {
                        return pMediaRes;
                    }
                }
            }
#endif

            return NULL;
        }

        return (CMediaRes*)CMediaDir::subPath(strOppPath, bDir);
    }
#endif

    CPath* _newSubDir(const tagFileInfo& fileInfo) override
    {
        if (fileInfo.strName.front() == L'.')
        {
            return NULL;
        }

        CMediaDir *pSubDir = new CMediaDir(fileInfo);
#if __windows
        if (m_fi.pParent && NULL == m_fi.pParent->parent())
#else
        if (NULL == m_fi.pParent)
#endif
        {
            if (!pSubDir->files())
            {
                cauto paDirs = pSubDir->dirs();
                if (paDirs.size() <= 1)
                {
                    if (!paDirs.any([&](CPath& subDir){
                        return subDir.dirs() || subDir.files();
                    }))
                    {
                        delete pSubDir;
                        return NULL;
                    }
                }
            }
        }

        return pSubDir;
    }
};

class CMedialibDlg : public CDialog
{
    Q_OBJECT
public:
    CMedialibDlg(QWidget& parent);

private:
    COuterDir m_OuterDir;

    CMedialibView m_lv;

    CWholeTrackDlg m_wholeTrackDlg;

    CSingerImgDlg m_singerImgDlg;

    size_t m_uRowCount = 0;

private slots:
    void slot_labelClick(class CLabel*, const QPoint&);

public:
    static size_t caleRowCount(int cy);

    size_t rowCount() const
    {
        return m_uRowCount;
    }

    void preinit();

    void init();

    void show();
    void showMediaSet(CMediaSet& MediaSet);
    bool showMedia(IMedia& media);
    CMediaRes* showMediaRes(cwstr strPath);

    void updateHead(const WString& strTitle);

    void updateSingerImg(cwstr strSingerName, const tagSingerImg& singerImg);

    bool tryShowWholeTrack(CMediaRes& mediaRes)
    {
        return m_wholeTrackDlg.tryShow(mediaRes);
    }

private:
    void _show();

    void _relayout(int cx, int cy) override;

    void _relayoutTitle() const;

    bool _handleReturn() override
    {
        return m_lv.upward();
    }
};
