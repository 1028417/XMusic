
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

class CImgDir : public CPath
{
public:
    using TD_SubImgList = vector<pair<XFile*, QPixmap>>;

public:
    CImgDir() {}

    CImgDir(const tagFileInfo& fileInfo) : CPath(fileInfo)
    {
    }

private:
    decltype(declval<TD_XFileList>().begin()) m_itrSubFile;

protected:
    QPixmap m_pmSnapshot;

    TD_SubImgList m_lstSubImgs;

public:
    QPixmap& snapshot()
    {
        return m_pmSnapshot;
    }

    bool genSnapshot();

    bool genSubImgs();

    const TD_SubImgList& subImgs()
    {
        return m_lstSubImgs;
    }

private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override;
    XFile* _newSubFile(const tagFileInfo& fileInfo) override;
};

using TD_ImgDirList = PtrArray<CImgDir>;

class CAddBkgView : public CListView
{
    Q_OBJECT

public:
    CAddBkgView(class CXMusicApp& app, class CAddBkgDlg& addbkgDlg, const TD_ImgDirList& paImgDir);

private:
    class CXMusicApp& m_app;

    class CAddBkgDlg& m_addbkgDlg;

    const TD_ImgDirList& m_paImgDirs;

    CImgDir *m_pImgDir = NULL;

private:
    size_t getColumnCount() override;

    size_t getPageRowCount() override;

    size_t getRowCount() override;

    void _onPaintRow(CPainter&, tagLVRow&) override;

    void _onRowClick(tagLVRow& lvRow, const QMouseEvent&) override;

public:
    bool isInRoot() const
    {
        return NULL==m_pImgDir;
    }

    bool upward();

    void clear()
    {
        m_pImgDir = NULL;
    }
};


class CAddBkgDlg : public CDialog
{
    Q_OBJECT
public:
    CAddBkgDlg(class CXMusicApp& app, class CBkgDlg& bkgDlg);

private:    
    class CXMusicApp& m_app;

    class CBkgDlg& m_bkgDlg;

    CAddBkgView m_addbkgView;

private:
    CImgDir m_ImgRoot;
    bool m_bScaning = false;
    std::thread m_thread;

    TD_ImgDirList m_paImgDirs;

signals:
    void signal_founddir(void* pDir);

private slots:
    void slot_founddir(void *pDir)
    {
        m_paImgDirs.add((CImgDir*)pDir);

        m_addbkgView.update();
    }

private:
    void _relayout(int cx, int cy) override;

    bool _handleReturn() override
    {
        return m_addbkgView.upward();
    }

    void _onClose() override;

public:
    void init();

    void show();

    void relayout()
    {
        _relayout(width(), height());
    }

    void addBkg(const wstring& strFile);
};
