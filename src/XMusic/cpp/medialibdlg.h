
#pragma once

#include "dialog.h"

#include "MedialibView.h"

#include "wholeTrackDlg.h"

#include "singerimgdlg.h"

class COuterDir : public CMediaDir
{
public:
    COuterDir()
#if __android
        : CMediaDir(L"/sdcard")
#elif !__windows
        : CMediaDir(fsutil::getHomeDir().toStdWString())
#endif
    {
    }

    COuterDir(cwstr strPath, class CPath *pParent)
        : CMediaDir(strPath, pParent)
    {
    }

public:
#if !__windows
    XFile* findSubFile(cwstr strSubFile)
    {
        cauto strOppPath = fsutil::GetOppPath(m_fi.strName, strSubFile);
        if (strOppPath.empty())
        {
            return NULL;
        }

        return CMediaDir::findSubFile(strOppPath);
    }
#endif

private:
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
#endif

    CPath* _newSubDir(const tagFileInfo& fileInfo) override
    {
        if (fileInfo.strName.front() == L'.')
        {
            return NULL;
        }

        CMediaDir *pSubDir = new CMediaDir(fileInfo);
        do {
#if __windows
            if (NULL == m_fi.pParent || m_fi.pParent->parent())
            {
                break;
            }
#else
            if (m_fi.pParent)
            {
                break;
            }
#endif

            cauto paDirs = pSubDir->dirs();
            if (paDirs.size() > 1 || pSubDir->files())
            {
                break;
            }

            bool bAvalid = false;
            paDirs.front([&](CPath& subDir){
                bAvalid = subDir.dirs() || subDir.files();
            });
            if (!bAvalid)
            {
                delete pSubDir;
                return NULL;
            }
        } while(0);

        return pSubDir;
    }
};

class CMedialibDlg : public CDialog
{
    Q_OBJECT
public:
    CMedialibDlg(QWidget& parent, class CApp& m_app);

private:
    class CApp& m_app;

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
    void showMedia(const CMedia& media);
    bool showMediaRes(cwstr strPath);

    void updateHead();

    void updateSingerImg(cwstr strSingerName, const tagSingerImg& singerImg);

    bool tryShowWholeTrack(CMediaRes& mediaRes)
    {
        return m_wholeTrackDlg.tryShow(mediaRes);
    }

private:
    void _show();

    void _relayout(int cx, int cy) override;

    void _resizeTitle() const;

    void _genTitle(CMediaSet&, WString& strTitle);
    void _genTitle(CPath&, WString& strTitle);

    bool _handleReturn() override
    {
        return m_lv.upward();
    }
};
