
#pragma once

#include "dialog.h"

#include "MedialibView.h"

#include "wholeTrackDlg.h"

class COuterDir : public CMediaDir
{
public:
    COuterDir() = default;

    COuterDir(const tagFileInfo& fileInfo, const wstring& strMediaLibDir)
        : CMediaDir(fileInfo)
        , m_strMediaLibDir(strMediaLibDir)
    {
    }

private:
    wstring m_strMediaLibDir;

    wstring m_strOuterDir;

public:
    wstring init();

    void findFile()
    {
        CPath::_findFile();
    }

private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override;

    wstring GetPath() const override
    {
        return m_strOuterDir + CMediaDir::GetPath();
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

    CMedialibView m_MedialibView;

    CWholeTrackDlg m_wholeTrackDlg;

private slots:
    void slot_labelClick(class CLabel*, const QPoint&);

public:
    size_t getPageRowCount() const;

    void init();

    void show();
    void showMediaSet(CMediaSet& MediaSet);
    void showMedia(CMedia& media);
    bool showMediaRes(const wstring& strPath);

    void updateHead(const WString& strTitle);

    void updateSingerImg()
    {
        m_MedialibView.updateSingerImg();
    }

    bool tryShowWholeTrack(CMediaRes& mediaRes)
    {
        return m_wholeTrackDlg.tryShow(mediaRes);
    }

private:
    void _relayout(int cx, int cy) override;

    void _resizeTitle() const;

    bool _handleReturn() override
    {
        return m_MedialibView.upward();
    }

    void _onClosed() override;
};
