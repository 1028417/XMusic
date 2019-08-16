#include "view.h"

#include "bkgdlg.h"
#include "ui_bkgdlg.h"

#include "mainwindow.h"

#define __margin    10

static Ui::BkgDlg ui;

CBkgView::CBkgView(CBkgDlg& bkgDlg)
    : CListView(&bkgDlg, 2, 2)
    , m_bkgDlg(bkgDlg)
{
}

UINT CBkgView::getRowCount()
{
    return 1+(1+m_bkgDlg.bkgCount())/2;
}

void CBkgView::_onPaintRow(CPainter& painter, QRect& rc, const tagListViewRow& lvRow)
{
    WString strImg;

    int nItem = lvRow.uRow * 2 + lvRow.uCol;
    if (0 == nItem)
    {
        strImg = L":/img/add.png";
    }
    else if (1 == nItem)
    {
        strImg = L":/img/add.png";
    }
    else
    {
        UINT uBkgIdx = (UINT)nItem-2;
        if (uBkgIdx >= m_bkgDlg.bkgCount())
        {
            return;
        }

        strImg = m_bkgDlg.bkg(uBkgIdx);
    }

    QPixmap pixmap;
    if (pixmap.load(strImg))
    {
        rc.setRect(rc.left()+__margin, rc.top()+__margin,rc.width()-__margin*2,rc.height()-__margin*2);
        painter.drawPixmapEx(pixmap, rc);
    }
}

void CBkgView::_handleRowClick(const tagListViewRow& lvRow, const QMouseEvent&)
{
    int nItem = lvRow.uRow * 2 + lvRow.uCol;
    if (0 == nItem)
    {
        if (m_bkgDlg.addBkg())
        {
            update(getRowCount());
        }
    }
    else if (1 == nItem)
    {
        m_bkgDlg.unsetBkg();
    }
    else
    {
        UINT uBkgIdx = (UINT)nItem-2;
        if (uBkgIdx >= m_bkgDlg.bkgCount())
        {
            return;
        }

        m_bkgDlg.setBkg(uBkgIdx);
    }
}

CBkgDlg::CBkgDlg(CPlayerView& view) :
    m_view(view),
    m_bkgView(*this)
{
    ui.setupUi(this);

    connect(ui.btnReturn, &CButton::signal_clicked, [&](CButton*){
        this->close();
    });
}

void CBkgDlg::init()
{
    m_strHBkgDir = fsutil::workDir() + L"/hbkg/";
    m_strVBkgDir = fsutil::workDir() + L"/vbkg/";

    if (!fsutil::existDir(m_strHBkgDir))
    {
        if (fsutil::createDir(m_strHBkgDir))
        {
            fsutil::copyFile(L"assets:/hbkg/win10.jpg", m_strHBkgDir + L"win10");
        }
    }

    if (!fsutil::existDir(m_strVBkgDir))
    {
        if (fsutil::createDir(m_strVBkgDir))
        {
            fsutil::copyFile(L"assets:/vbkg/win10.jpg", m_strVBkgDir + L"win10");
        }
    }

    fsutil::findFile(m_strHBkgDir, [&](const tagFileInfo& fileInfo) {
        m_vecHBkg.push_back(fileInfo.m_strName);
    });

    fsutil::findFile(m_strVBkgDir, [&](const tagFileInfo& fileInfo) {
        m_vecVBkg.push_back(fileInfo.m_strName);
    });
}

WString CBkgDlg::hbkg() const
{
    cauto& strBkg = m_view.getOptionMgr().getOption().strHBkg;
    if (strBkg.empty())
    {
        return L"";
    }

    return m_strHBkgDir + strBkg;
}

WString CBkgDlg::vbkg() const
{
    cauto& strBkg = m_view.getOptionMgr().getOption().strVBkg;
    if (strBkg.empty())
    {
        return L"";
    }

    return m_strVBkgDir + strBkg;
}

void CBkgDlg::_relayout(int cx, int cy)
{
    m_bHScreen = cx>cy;

    int offset = ui.btnReturn->geometry().bottom() + __margin;
    if (m_bHScreen)
    {
        int cx_bkgView = cx-__margin-offset;
        int cy_bkgView = cx_bkgView*cy/cx;
        m_bkgView.setGeometry(offset, (cy-cy_bkgView)/2, cx_bkgView, cy_bkgView);
    }
    else
    {
        int cy_bkgView = cy-__margin-offset;
        int cx_bkgView = cy_bkgView*cx/cy;
        m_bkgView.setGeometry((cx-cx_bkgView)/2, offset, cx_bkgView, cy_bkgView);
    }
}

WString CBkgDlg::bkg(UINT uIdx)
{
    auto& vecBkg = m_bHScreen?m_vecHBkg:m_vecVBkg;
    if (uIdx >= vecBkg.size())
    {
        return L"";
    }

    cauto& stBkgDir = m_bHScreen?m_strHBkgDir:m_strVBkgDir;
    return stBkgDir + vecBkg[uIdx];
}

bool CBkgDlg::addBkg()
{
    return true;
}

void CBkgDlg::setBkg(UINT uIdx)
{
    auto& vecBkg = m_bHScreen?m_vecHBkg:m_vecVBkg;
    if (uIdx >= vecBkg.size())
    {
        return;
    }

    auto& strBkg = m_bHScreen?m_view.getOptionMgr().getOption().strHBkg
                            :m_view.getOptionMgr().getOption().strVBkg;
    strBkg = vecBkg[uIdx];

    m_view.getMainWnd().loadBkg(m_bHScreen?m_strHBkgDir:m_strVBkgDir + strBkg);

    close();
}

void CBkgDlg::unsetBkg()
{
    m_view.getMainWnd().loadBkg(L"");

    close();
}
