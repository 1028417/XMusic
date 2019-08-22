
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

public:
    bool isInRoot() const
    {
        return NULL == m_pDir;
    }

    void upward()
    {
        if (m_pDir)
        {
            m_pDir = NULL;

            update();
        }
    }

private:
    UINT getRowCount() override;

    UINT getColumnCount() override;

    void _onPaintRow(CPainter&, QRect&, const tagLVRow&) override;
};

class CAddBkgDlg : public CDialog
{
    Q_OBJECT
public:
    explicit CAddBkgDlg();

private:
    CAddBkgView m_addbkgView;

    CPath m_sdcard;
    TD_PathList m_paDirs;

    std::thread m_thread;
    bool m_bCancel = false;

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
