
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

class CImgDir : public CPath
{
public:
    using TD_SubImgList = vector<pair<CPath*, QPixmap>>;

public:
    CImgDir(const wstring& strDir) : CPath(strDir)
    {
    }

    CImgDir(const tagFileInfo& FileInfo) : CPath(FileInfo)
    {
    }

private:
    decltype(declval<TD_PathList>().begin()) m_itrSubFile;

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
    CPath* _newSubPath(const tagFileInfo& FileInfo) override;
};

class CImgRoot : public CImgDir
{
public:
    CImgRoot() : CImgDir(L"/sdcard")
    {
    }

private:
   bool m_bScaning = false;

   std::thread m_thread;

public:
   void startScan(const function<void(CPath& dir)>& cb)
   {
       if (m_thread.joinable())
       {
           return;
       }

       m_bScaning = true;

       m_thread = thread([=](){
           CPath::scan([&](CPath& dir, TD_PathList& paSubFile){
               if (paSubFile)
               {
                   cb(dir);
               }

               return m_bScaning;
           });
       });
   }

   void stopScan()
   {
       m_bScaning = false;

       if (m_thread.joinable())
       {
           m_thread.join();
       }

       CPath::Clear();

       m_pmSnapshot = QPixmap();

       m_lstSubImgs.clear();
   }
};

using TD_ImgDirList = PtrArray<CImgDir>;

class CAddBkgView : public CListView
{
    Q_OBJECT

public:
    CAddBkgView(class CPlayerView& view, class CAddBkgDlg& addbkgDlg, const TD_ImgDirList& paImgDir);

private:
    class CPlayerView& m_view;

    class CAddBkgDlg& m_addbkgDlg;

    const TD_ImgDirList& m_paImgDirs;

    CImgDir *m_pImgDir = NULL;

private:
    UINT getColumnCount() override;

    UINT getPageRowCount() override;

    UINT getRowCount() override;

    void _onPaintRow(CPainter&, QRect&, const tagLVRow&) override;

    void _onRowClick(const tagLVRow& lvRow, const QMouseEvent&) override;

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
    explicit CAddBkgDlg(class CPlayerView& view, class CBkgDlg& bkgDlg);

private:
    class CPlayerView& m_view;

    class CBkgDlg& m_bkgDlg;

    CAddBkgView m_addbkgView;

    CImgRoot m_ImgRoot;

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
    void _relayout(int cx, int cy);

    bool _handleReturn();

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
