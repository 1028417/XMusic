
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

class IImgDir
{
public:
    IImgDir() = default;

    virtual ~IImgDir(){}

    virtual size_t imgCount() const = 0;

    virtual const QPixmap* snapshot(int nIdx=-1) const = 0;

    virtual wstring path(int nIdx=-1) const = 0;

    virtual bool genSubImgs() = 0;
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
    size_t getColumnCount() const override;

    size_t getPageRowCount() const override;

    size_t getRowCount() const override;

    void _onPaintRow(CPainter&, tagLVRow&) override;

    void _onRowClick(tagLVRow& lvRow, const QMouseEvent&) override;

public:
    void showImgDir(IImgDir& imgDir);

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
    CAddBkgDlg(class CBkgDlg& bkgDlg, class CApp& app);

private:
    class CBkgDlg& m_bkgDlg;

    class CApp& m_app;

    CAddBkgView m_addbkgView;

private:
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

public:
    class CBkgDlg& bkgDlg()
    {
        return m_bkgDlg;
    }

    void init();

    void show(IImgDir *pImgDir, cfn_void cbClose = NULL);

    void relayout()
    {
        _relayout(width(), height());
    }

    void addImgDir(IImgDir& imgDir)
    {
        emit signal_founddir(&imgDir);
    }
};
