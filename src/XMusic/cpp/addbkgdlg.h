
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

using TD_SubImgVector = vector<pair<QPixmap, wstring>>;

class IImgDir
{
public:
    IImgDir(){}

    virtual ~IImgDir(){}

    virtual const QPixmap* snapshot() const {return NULL;}

    virtual wstring fileName() const {return L"";}

    virtual bool genSubImgs() = 0;

    virtual const TD_SubImgVector& subImgs() const = 0;
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
    CAddBkgDlg(class CApp& app, class CBkgDlg& bkgDlg);

private:    
    class CApp& m_app;

    class CBkgDlg& m_bkgDlg;

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
    void init();

    void show(IImgDir& imgDir, bool bRoot, cfn_void cbClose = NULL);

    void relayout()
    {
        _relayout(width(), height());
    }

    void addImgDir(IImgDir& imgDir)
    {
        emit signal_founddir(&imgDir);
    }

    void addBkg(const wstring& strFile);
};
