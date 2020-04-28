
#pragma once

#include "dialog.h"

#include "listview.h"

#include "addbkgdlg.h"

#include "colordlg.h"

class CBkgView : public CListView
{
public:
    CBkgView(class CApp& app, class CBkgDlg& bkgDlg);

private:
    class CApp& m_app;

    class CBkgDlg& m_bkgDlg;

    QPixmap m_pmX;

private:
    size_t getPageRowCount() const override;

    size_t getColumnCount() const override;

    size_t getRowCount() const override;

    void _onPaintRow(CPainter&, tagLVRow&) override;

    void _onRowClick(tagLVRow&, const QMouseEvent&) override;

public:
    UINT margin();
};

class CImgDir : public CPath, public IImgDir
{
public:
    CImgDir() = default;

    CImgDir(const tagFileInfo& fileInfo) : CPath(fileInfo)
    {
    }

private:
    XThread m_thread;

    decltype(declval<TD_XFileList>().begin()) m_itrSubFile;

    QPixmap m_pmSnapshot;

    vector<pair<QPixmap, wstring>> m_vecSubImgs;

public:
    void scan(const wstring& strDir, cfn_void_t<CImgDir&> cb);

private:
    size_t imgCount() const override
    {
        return m_vecSubImgs.size();
    }

    const QPixmap* snapshot(int nIdx) const override;

    wstring path(int nIdx) const override;

    bool genSubImgs() override;

private:
    void _onClear() override;

    CPath* _newSubDir(const tagFileInfo& fileInfo) override
    {
        return new CImgDir(fileInfo);
    }

    XFile* _newSubFile(const tagFileInfo& fileInfo) override;

    bool _genSnapshot(TD_XFileList& paSubFile);
};

class CBkgBrush : public QBrush
{
public:
    CBkgBrush()=default;

    CBkgBrush(cqpm pm)
        : QBrush(pm)
    {
        m_cx = pm.width();
        m_cy = pm.height();
    }

    int m_cx = 0;
    int m_cy = 0;
};

struct tagBkgFile
{
    tagBkgFile() = default;

    tagBkgFile(bool bInner, const wstring& strPath, CBkgBrush *br=NULL)
        : bInner(bInner)
        , strPath(strPath)
        , br(br)
    {
    }

    bool bInner = false;
    WString strPath;
    CBkgBrush *br = NULL;
};

class CBkgDlg : public CDialog
{
    Q_OBJECT
public:
    CBkgDlg(QWidget& parent, class CApp& app);

private:
    class CApp& m_app;

    CColorDlg m_colorDlg;

    TD_ImgDirList m_paImgDirs;

    CAddBkgDlg m_addbkgDlg;

    CBkgView m_lv;

    wstring m_strHBkgDir;
    wstring m_strVBkgDir;

    vector<tagBkgFile> m_vecHBkgFile;
    vector<tagBkgFile> m_vecVBkgFile;

    list<CBkgBrush> m_lstBr;

    QPixmap m_pmHBkg;
    QPixmap m_pmVBkg;

    CImgDir m_rootImgDir;

signals:
    void signal_founddir(void* pDir);

private slots:
    void slot_founddir(void *pDir)
    {
        m_paImgDirs.add((IImgDir*)pDir);

        m_addbkgDlg.update();
    }

private:
    void _addImgDir(IImgDir& imgDir)
    {
        emit signal_founddir(&imgDir);
    }

    wstring& _bkgDir();

    vector<tagBkgFile>& _vecBkgFile();

    CBkgBrush& _addbr(QPixmap& pm);

    void _relayout(int cx, int cy) override;

    void _setBkg(const wstring& strFile);

    QPixmap _loadBkg(const WString& strFile);

    void _updateBkg(const wstring& strFile);

    void _showAddBkg();

    void _onClosed() override;

public:
    void preinit();

    void init();

    cqpm hbkg() const
    {
        return m_pmHBkg;
    }
    cqpm vbkg() const
    {
        return m_pmVBkg;
    }

    size_t bkgCount() const;

    CBkgBrush* brush(size_t uIdx);

    void setBkg(size_t uItem);

    void addBkg(const wstring& strFile);

    void deleleBkg(size_t uIdx);
};
