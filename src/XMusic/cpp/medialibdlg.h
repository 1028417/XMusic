
#pragma once

#include "dialog.h"

#include "MedialibView.h"

class COuterDir : public CMediaDir
{
public:
    COuterDir() {}

    COuterDir(const tagFileInfo& fileInfo, const wstring& strMediaLibDir)
        : CMediaDir(fileInfo)
        , m_strMediaLibDir(strMediaLibDir)
    {
    }

private:
    wstring m_strMediaLibDir;

    wstring m_strOuterDir;

public:
    wstring init(const wstring& strMediaLibDir);

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

public:
    size_t getPageRowCount();

    void init();

    void show();
    void showMediaSet(CMediaSet& MediaSet);
    void showMedia(CMedia& media);
    bool showFile(const wstring& strPath);

    void updateHead(const wstring& strTitle, bool bShowPlayButton, bool bShowUpwardButton, bool bAutoFitText=true);

    void updateSingerImg()
    {
        m_MedialibView.updateSingerImg();
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
