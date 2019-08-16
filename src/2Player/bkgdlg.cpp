#include "view.h"

#include "bkgdlg.h"
#include "ui_bkgdlg.h"

static Ui::BkgDlg ui;

CBkgView::CBkgView(CBkgDlg& bkgDlg)
    : CListView(&bkgDlg)
    , m_bkgDlg(bkgDlg)
{
}

UINT CBkgView::getItemCount()
{
    return m_bkgDlg.bkgs().size()+1;
}

void CBkgView::_onPaintItem(CPainter&, QRect&, const tagListViewItem& lvitem)
{
    auto& bkgs = m_bkgDlg.bkgs();
    if (0 == lvitem.uItem)
    {

    }
    else
    {
        UINT uItem = lvitem.uItem-1;
        if (uItem < bkgs.size())
        {
            auto& strBkg = bkgs[uItem];
            QPixmap pixmap;
            if (pixmap.load(strBkg))
            {

            }
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

    cauto& rcReturn = ui.btnReturn->geometry();
    int y_margin = rcReturn.top();

    int y_bkgView = rcReturn.bottom() + y_margin;
    m_bkgView.setGeometry(0, y_bkgView, cx, cy-y_bkgView);
}
