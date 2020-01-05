
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

class IImgDir
{
protected:
    using TD_SubImgList = vector<pair<XFile*, QPixmap>>;

public:
    IImgDir(){}

    virtual ~IImgDir(){}

    virtual const QPixmap& snapshot() const = 0;

    virtual wstring fileName() const = 0;

    virtual bool genSubImgs() = 0;

    virtual const TD_SubImgList& subImgs() const = 0;
};

using TD_ImgDirList = PtrArray<IImgDir>;

class CAddBkgView : public CListView
{
    Q_OBJECT
public:
    CAddBkgView(class CApp& app, class CAddBkgDlg& addbkgDlg, const TD_ImgDirList& paImgDir);

private:
    class CApp& m_app;

    class CAddBkgDlg& m_addbkgDlg;

    const TD_ImgDirList& m_paImgDirs;

    IImgDir *m_pImgDir = NULL;

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


class CImgDir : public CPath, public IImgDir
{
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
    void scan(bool& bRunFlag, cfn_void_t<IImgDir&> cb);

    const QPixmap& snapshot() const override
    {
        return m_pmSnapshot;
    }

    wstring fileName() const override
    {
        return CPath::oppPath();
    }

    bool genSubImgs() override;

    const TD_SubImgList& subImgs() const override
    {
        return m_lstSubImgs;
    }

private:
    void _onClear() override
    {
        m_lstSubImgs.clear();
    }

    CPath* _newSubDir(const tagFileInfo& fileInfo) override
    {
        return new CImgDir(fileInfo);
    }

    XFile* _newSubFile(const tagFileInfo& fileInfo) override;

    bool _genSnapshot();
};


class CAddBkgDlg : public CDialog
{
    Q_OBJECT
public:
    CAddBkgDlg(class CApp& app, class CBkgDlg& bkgDlg);

private:    
    class CApp& m_app;

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
        m_paImgDirs.add((IImgDir*)pDir);

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
