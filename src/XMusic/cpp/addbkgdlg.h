
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


class CImgRoot : public CImgDir
{
public:
    CImgRoot() {}

private:
   bool m_bScaning = false;

   std::thread m_thread;

public:
   void startScan(const wstring& strDir, const function<void(CPath& dir)>& cb)
   {
       SetDir(strDir);

       if (m_thread.joinable())
       {
           return;
       }

       m_bScaning = true;

       m_thread = thread([=](){
           CPath::scan([&](CPath& dir, TD_XFileList& paSubFile){
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

    void _onPaintRow(CPainter&, const tagLVRow&) override;

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
    explicit CAddBkgDlg(class CXMusicApp& app, class CBkgDlg& bkgDlg);

private:    
    class CXMusicApp& m_app;

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
    void _relayout(int cx, int cy) override;

    bool _handleReturn() override;

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
