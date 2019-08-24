
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

class CAddBkgView : public CListView
{
    Q_OBJECT

public:
    CAddBkgView(class CAddBkgDlg& addbkgDlg, const TD_PathList& m_paDirs);

private:
    class CAddBkgDlg& m_addbkgDlg;

    const TD_PathList& m_paDirs;

    CPath *m_pDir = NULL;

    ArrList<QPixmap> m_paPixmap;

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
    virtual CPath* NewSubPath(const tagFileInfo& FileInfo)
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

    TD_PathList m_paDirs;
    map<void*, QPixmap> m_mapPixmaxp;

signals:
    void signal_founddir(void* pDir);

private slots:
    void slot_founddir(void *pDir)
    {
        m_paDirs.add((CPath*)pDir);

        QPixmap& pm = m_mapPixmaxp[pDir];
        for (auto pSubFile : ((CPath*)pDir)->files())
        {
            pm.load(wsutil::toQStr(pSubFile->absPath()));
            break;
        }

        m_addbkgView.update();
    }

private:
    void _relayout(int cx, int cy);

    bool _handleReturn();

    void close();

public:
    void init();

    void show();

    const QPixmap* getPixmap(CPath& path);
};
