#include "view.h"

#include "bkgdlg.h"
#include "ui_bkgdlg.h"

#include "mainwindow.h"

#define __margin    50

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

void CBkgView::_onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow)
{
    if (0 == lvRow.uRow)
    {
        rc.setBottom(rc.bottom()-__margin/2);
    }
    else
    {
        rc.setTop(rc.top()+__margin/2);
    }

    if (0 == lvRow.uCol)
    {
        rc.setRight(rc.right()-__margin/2);
    }
    else
    {
        rc.setLeft(rc.left()+__margin/2);
    }

    int nItem = lvRow.uRow * 2 + lvRow.uCol;
    if (0 == nItem)
    {
        m_bkgDlg.paintDefaultBkg(painter, rc);
   }
    else
    {
        UINT uIdx = (UINT)nItem-1;
        cauto& strBkg = m_bkgDlg.bkg(uIdx);
        if (!strBkg.empty())
        {
            painter.drawPixmapEx(rc, strBkg);
        }
        else
        {
            QPixmap pmAdd;
            if (pmAdd.load(":/img/add.png"))
            {
#define __size  80
                painter.drawPixmap(rc.center().x()-__size, rc.center().y()-__size, __size*2, __size*2, pmAdd);
            }

            painter.drawFrame(8, rc, 255,255,255,255, Qt::BrushStyle::Dense7Pattern);
        }
    }
}

void CBkgView::_onRowClick(const tagLVRow& lvRow, const QMouseEvent&)
{
    int nItem = lvRow.uRow * 2 + lvRow.uCol;
    if (0 == nItem)
    {
        m_bkgDlg.unsetBkg();
    }
    else
    {
        UINT uBkgIdx = (UINT)nItem-1;
        if (uBkgIdx < m_bkgDlg.bkgCount())
        {
            m_bkgDlg.setBkg(uBkgIdx);
        }
        else
        {
            if (m_bkgDlg.addBkg())
            {
                scroll(getRowCount());
            }
        }
    }
}

CBkgDlg::CBkgDlg(CPlayerView& view) :
    m_view(view),
    m_bkgView(*this)
{
}

void CBkgDlg::init()
{
    ui.setupUi(this);

    QColor crText(32, 128, 255);
    ui.labelTitle->setTextColor(crText);
    ui.labelTitle->setFont(2, E_FontWeight::FW_SemiBold);

    connect(ui.btnReturn, &CButton::signal_clicked, [&](CButton*) {
        this->close();
    });


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

    fsutil::findSubFile(m_strHBkgDir, [&](const wstring& strSubFile) {
        m_vecHBkg.push_back(strSubFile);
    });

    fsutil::findSubFile(m_strVBkgDir, [&](const wstring& strSubFile) {
        m_vecVBkg.push_back(strSubFile);
    });

    cauto& strHBkg = m_view.getOptionMgr().getOption().strHBkg;
    if (!strHBkg.empty())
    {
        (void)m_pmHBkg.load(m_strHBkgDir + strHBkg);
    }

    cauto& strVBkg = m_view.getOptionMgr().getOption().strVBkg;
    if (!strVBkg.empty())
    {
        (void)m_pmVBkg.load(m_strVBkgDir + strVBkg);
    }

    m_addbkgDlg.init();
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

        ui.labelTitle->setVisible(false);
    }
    else
    {
        int cy_bkgView = cy-__margin-offset;
        int cx_bkgView = cy_bkgView*cx/cy;
        m_bkgView.setGeometry((cx-cx_bkgView)/2, offset, cx_bkgView, cy_bkgView);

        ui.labelTitle->move((cx-ui.labelTitle->width())/2, ui.labelTitle->y());
        ui.labelTitle->setVisible(true);
    }
}

void CBkgDlg::paintDefaultBkg(QPainter& painter, const QRect& rcDst)
{
    QRect rcSrc = m_pmDefaultBkg.rect();

    float fHWRate = (float)rcDst.height()/rcDst.width();
    if (fHWRate < 1)
    {
        rcSrc.setTop(rcSrc.bottom()-rcSrc.width()*fHWRate);
    }
    else
    {
    #define __offset    1080
        rcSrc.setRight(__offset);
        rcSrc.setTop(rcSrc.bottom()-rcSrc.right()*fHWRate);
    }

    painter.drawPixmap(rcDst, m_pmDefaultBkg, rcSrc);
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

    cauto& stBkgDir = m_bHScreen?m_strHBkgDir:m_strVBkgDir;
    QPixmap& pmBkg = m_bHScreen? m_pmHBkg:m_pmVBkg;
    (void)pmBkg.load(stBkgDir + strBkg);
    m_view.getMainWnd().updateBkg();

    close();
}

void CBkgDlg::unsetBkg()
{
    QPixmap& pmBkg = m_bHScreen? m_pmHBkg:m_pmVBkg;
    pmBkg = QPixmap();
    m_view.getMainWnd().updateBkg();

    close();
}

bool CBkgDlg::addBkg()
{
    m_addbkgDlg.show();

    return true;
}
