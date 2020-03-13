
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
    void setDir(const wstring& strMediaLibDir, const wstring& strOuterDir);

private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override;

    wstring GetPath() const override
    {
        return m_strOuterDir + CMediaDir::GetPath();
    }

    CMediaRes* findSubFile(const wstring& strSubFile) override;
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

    void show()
    {
        _initOuter();

        m_MedialibView.showRoot();

        _show();
    }

    void showMediaSet(CMediaSet& MediaSet)
    {
        _initOuter();

        m_MedialibView.showMediaSet(MediaSet);

        _show();
    }

    void showMedia(CMedia& media)
    {
        _initOuter();

        m_MedialibView.showMedia(media);

        _show();
    }

    bool showFile(const wstring& strPath)
    {
        _initOuter();

        if (!m_MedialibView.showFile(strPath))
        {
            return false;
        }

        _show();

        return true;
    }

    void updateHead(const wstring& strTitle, bool bShowPlayButton, bool bShowUpwardButton, bool bAutoFitText=true);

    void updateSingerImg()
    {
        m_MedialibView.updateSingerImg();
    }

private:
    void _initOuter();

    void _show();

    void _relayout(int cx, int cy) override;

    void _resizeTitle() const;

    bool _handleReturn() override
    {
        return m_MedialibView.upward();
    }

    void _onClosed() override;
};
