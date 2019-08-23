
#pragma once

#include "dialog.h"

#include "ListViewEx.h"

class CAddBkgView : public CListView
{
    Q_OBJECT

public:
    CAddBkgView(class CAddBkgDlg& addbkgDlg, const TD_PathList& paDirs);

private:
    class CAddBkgDlg& m_addbkgDlg;

    const TD_PathList& m_paDirs;

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


class CImgRoot : public CPath
{
public:
    CImgRoot() : CPath(L"/sdcard")
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
    explicit CAddBkgDlg();

private:
    CAddBkgView m_addbkgView;

    CImgRoot m_ImgRoot;

    TD_PathList m_paDirs;

signals:
    void signal_founddir(void* pDir);

private slots:
    void slot_founddir(void *pDir)
    {
        m_paDirs.add((CPath*)pDir);
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
