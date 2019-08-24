
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

using TD_ImgDirList = PairList<CPath*, const QPixmap*>;

class CAddBkgView : public CListView
{
    Q_OBJECT

public:
    CAddBkgView(class CAddBkgDlg& addbkgDlg, const TD_ImgDirList& plDirs);

private:
    class CAddBkgDlg& m_addbkgDlg;

    const TD_ImgDirList& m_plDirs;

    CPath *m_pDir = NULL;

private:
    inline UINT _picLayoutCount() const;

    UINT getColumnCount() override;

    UINT getPageRowCount() override;

    UINT getRowCount() override;

    void _onPaintRow(CPainter&, QRect&, const tagLVRow&) override;

    void _onRowClick(const tagLVRow& lvRow, const QMouseEvent&) override;

public:
    bool upward();
};


extern const SSet<wstring>& g_setImgExtName;

class CImgDir : public CPath
{
public:
    CImgDir(const wstring& strDir) : CPath(strDir)
    {
    }

    CImgDir(const tagFileInfo& FileInfo) : CPath(FileInfo)
    {
    }

private:
    CPath* _newSubPath(const tagFileInfo& FileInfo) override
    {
        if (FileInfo.bDir)
        {
            return new CImgDir(FileInfo);
        }

        cauto& strExtName = wsutil::lowerCase_r(fsutil::GetFileExtName(FileInfo.strName));
        if (g_setImgExtName.includes(strExtName))
        {
            return new CPath(FileInfo);
        }

        return NULL;
    }
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
   }
};

class CAddBkgDlg : public CDialog
{
    Q_OBJECT
public:
    explicit CAddBkgDlg(class CBkgDlg& bkgDlg);

private:
    class CBkgDlg& m_bkgDlg;

    CAddBkgView m_addbkgView;

    CImgRoot m_ImgRoot;

    list<QPixmap> m_lstPixmap;

    TD_ImgDirList m_plDirs;

signals:
    void signal_founddir(void* pDir, QPixmap *pm);

private slots:
    void slot_founddir(void *pDir, QPixmap *pm)
    {
        m_plDirs.add((CPath*)pDir, pm);

        m_addbkgView.update();
    }

private:
    void _relayout(int cx, int cy);

    bool _handleReturn();

    void close();

public:
    void init();

    void show();
};
