
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

class CAddBkgView : public CListView
{
    Q_OBJECT

public:
    CAddBkgView(class CAddBkgDlg& addbkgDlg);

private:
    class CAddBkgDlg& m_addbkgDlg;

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

    TD_PathList m_paDirs;
    ArrList<QPixmap> m_alPixmap;

    map<void*, QPixmap> m_mapPixmaxp;

signals:
    void signal_founddir(void* pDir);

private slots:
    void slot_founddir(void *pDir)
    {
        for (auto pSubFile : ((CPath*)pDir)->files())
        {
            QPixmap pm;
            if (!pm.load(wsutil::toQStr(pSubFile->absPath())))
            {
                continue;
            }

#define __filterSize 300
            if (pm.width()<__filterSize || pm.height()<__filterSize)
            {
                continue;
            }

#define __zoomoutSize 150
            CPainter::zoomoutPixmap(pm, __zoomoutSize);

            m_alPixmap.add(pm);
            m_paDirs.add((CPath*)pDir);

            m_addbkgView.update();

            break;
        }
    }

private:
    void _relayout(int cx, int cy);

    bool _handleReturn();

    void close();

public:
    void init();

    void show();

    const TD_PathList& dirs() const
    {
        return m_paDirs;
    }

    const ArrList<QPixmap>& pixmap() const
    {
        return m_alPixmap;
    }

    const QPixmap* getPixmap(CPath& path);
};
