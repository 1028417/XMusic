
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
    size_t getPageRowCount() override;

    size_t getColumnCount() override;

    size_t getRowCount() override;

    void _onPaintRow(CPainter&, tagLVRow&) override;

    void _onRowClick(tagLVRow&, const QMouseEvent&) override;

public:
    UINT margin();
};

class COnlineImgDir : public IImgDir
{
public:
    COnlineImgDir()
    {
    }

public:
    void downlaod()
    {

    }

private:
    TD_SubImgVector m_vecImgs;

private:
    bool genSubImgs() override
    {
        return false;
    }

    const TD_SubImgVector& subImgs() const override
    {
        return m_vecImgs;
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

    QPixmap m_pmSnapshot;

    TD_SubImgVector m_lstSubImgs;

public:
    bool genSnapshot();

    const QPixmap* snapshot() const override
    {
        return &m_pmSnapshot;
    }

    wstring fileName() const override
    {
        return CPath::oppPath();
    }

    bool genSubImgs() override;

    const TD_SubImgVector& subImgs() const override
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
};

class CBkgDlg : public CDialog
{
    Q_OBJECT
public:
    CBkgDlg(class CApp& app);

private:
    class CApp& m_app;

    CColorDlg m_colorDlg;

    CAddBkgDlg m_addbkgDlg;

    CBkgView m_bkgView;

    WString m_strHBkgDir;
    WString m_strVBkgDir;

    vector<WString> m_vecHBkgFile;
    vector<WString> m_vecVBkgFile;

    list<QPixmap> m_lstPixmap;
    vector<const QPixmap*> m_vecHPixmap;
    vector<const QPixmap*> m_vecVPixmap;

    QPixmap m_pmHBkg;
    QPixmap m_pmVBkg;

    bool m_bHScreen = false;

    CImgDir m_rootImgDir;
    COnlineImgDir m_olImgDir;

    XThread m_thread;

private:
    inline WString& _bkgDir()
    {
        return m_bHScreen?m_strHBkgDir:m_strVBkgDir;
    }

    inline vector<WString>& _vecBkgFile()
    {
        return m_bHScreen?m_vecHBkgFile:m_vecVBkgFile;
    }

    inline vector<const QPixmap*>& _vecPixmap()
    {
        return m_bHScreen?m_vecHPixmap:m_vecVPixmap;
    }

    void _relayout(int cx, int cy) override;

    void _setBkg(const wstring& strFile);

    void _showAddBkg();

    void _onClose() override;

public:
    void init();

    void show();

    const QPixmap& hbkg() const
    {
        return m_pmHBkg;
    }
    const QPixmap& vbkg() const
    {
        return m_pmVBkg;
    }

    size_t bkgCount() const
    {
        return (m_bHScreen?m_vecHBkgFile:m_vecVBkgFile).size()+1;
    }

    const QPixmap* pixmap(size_t uIdx);

    void setBkg(size_t uIdx);

    void addBkg(const wstring& strFile);

    void deleleBkg(size_t uIdx);
};
