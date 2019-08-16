#include "view.h"

#include "bkgdlg.h"
#include "ui_bkgdlg.h"

static Ui::BkgDlg ui;

CBkgView::CBkgView(CBkgDlg& bkgDlg)
    : CListView(&bkgDlg, 2, 2)
    , m_bkgDlg(bkgDlg)
{
}

UINT CBkgView::getRowCount()
{
    return 1+(1+m_bkgDlg.bkgs().size())/2;
}

void CBkgView::_onPaintItem(CPainter& painter, QRect& rc, const tagListViewRow& lvRow)
{
    int nItem = lvRow.uRow * 2 + lvRow.uCol;
    if (0 == nItem)
    {
        return;
    }
    else if (1 == nItem)
    {

    }
    else
    {
        UINT uItem = (UINT)nItem-2;

        auto& bkgs = m_bkgDlg.bkgs();
        if (uItem >= bkgs.size())
        {
            return;
        }

#define __margin    2
        rc.intersects(QRect(__margin,__margin,__margin,__margin));

        auto& strBkg = bkgs[uItem];
        QPixmap pixmap;
        if (pixmap.load(m_bkgDlg.bkgDir() + strBkg))
        {
            painter.drawPixmapEx(rc, pixmap);
        }
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

    int margin = ui.btnReturn->y();
    int offset = margin*2+ui.btnReturn->height();
    if (m_bHScreen)
    {
        int cx_bkgView = cx-margin-offset;
        int cy_bkgView = cx_bkgView*cy/cx;
        m_bkgView.setGeometry(offset, (cy-cy_bkgView)/2, cx_bkgView, cy_bkgView);
    }
    else
    {
        int cy_bkgView = cy-margin-offset;
        int cx_bkgView = cy_bkgView*cx/cy;
        m_bkgView.setGeometry((cx-cx_bkgView)/2, offset, cx_bkgView, cy_bkgView);
    }
}
