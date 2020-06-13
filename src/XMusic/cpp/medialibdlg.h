
#pragma once

#include "dialog.h"

#include "MedialibView.h"

#include "wholeTrackDlg.h"

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

        return new CMediaDir(fileInfo);
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
    void showMedia(CMedia& media);
    bool showMediaRes(cwstr strPath);

    void updateHead(const WString& strTitle);

    void updateSingerImg()
    {
        m_lv.updateSingerImg();
    }

    bool tryShowWholeTrack(CMediaRes& mediaRes)
    {
        return m_wholeTrackDlg.tryShow(mediaRes);
    }

private:
    void _show();

    void _relayout(int cx, int cy) override;

    void _resizeTitle() const;

    bool _handleReturn() override
    {
        return m_lv.upward();
    }
};
