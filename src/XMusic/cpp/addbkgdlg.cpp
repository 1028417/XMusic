#include "app.h"

#include "bkgdlg.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

CAddBkgDlg::CAddBkgDlg(CBkgDlg& bkgDlg, const TD_ImgDirList& paImgDirs)
    : CDialog(bkgDlg)
    , m_bkgDlg(bkgDlg)
    , m_lv(*this, paImgDirs)
{
}

void CAddBkgDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &CAddBkgDlg::_handleReturn);
}

void CAddBkgDlg::show(cfn_void cbClose)
{
    //if (pImgDir) m_lv.showImgDir(*pImgDir);

    CDialog::show([=](){
        if (cbClose)
        {
            cbClose();
        }

        m_lv.upward();
    });
}

void CAddBkgDlg::_relayout(int cx, int cy)
{
    int sz = MAX(cx, cy)/11;
    int xMargin = sz/4;
    QRect rcReturn(xMargin, xMargin, sz-xMargin*2, sz-xMargin*2);
    if (CApp::checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        rcReturn.moveTop(__cyIPhoneXBangs - rcReturn.top());
    }
    ui.btnReturn->setGeometry(rcReturn);

    ui.labelTitle->move(rcReturn.right() + xMargin, rcReturn.center().y() - ui.labelTitle->height()/2);

    int y_addbkgView = 0;
    if (m_lv.isInRoot())
    {
        y_addbkgView = rcReturn.bottom() + rcReturn.top();
        ui.labelTitle->setVisible(true);
    }
    else
    {
        ui.labelTitle->setVisible(false);
    }
    m_lv.setGeometry(0, y_addbkgView, cx, cy-y_addbkgView);

    static BOOL bHLayout = -1;
    if (bHLayout != (BOOL)m_bHLayout)
    {
        bHLayout = m_bHLayout;
        m_lv.scroll(m_lv.scrollPos());
    }
}

bool CAddBkgDlg::_handleReturn()
{
    if (m_lv.isInRoot())
    {
        close();
    }
    else
    {
        m_lv.upward();
        relayout();
    }

    return true;
}

CAddBkgView::CAddBkgView(CAddBkgDlg& addbkgDlg, const TD_ImgDirList& paImgDir) :
    CListView(addbkgDlg, E_LVScrollBar::LVSB_Left)
    , m_addbkgDlg(addbkgDlg)
    , m_paImgDirs(paImgDir)
{
}

size_t CAddBkgView::getColCount() const
{
    if (m_pImgDir)
    {
        if (m_pImgDir->imgCount() <= 4)
        {
            return 2;
        }
        else
        {
            return 3;
        }
    }
    else
    {
        if (m_addbkgDlg.isHLayout() && getItemCount() > getRowCount())
        {
            return 2;
        }

        return 1;
    }
}

size_t CAddBkgView::getRowCount() const
{
    if (m_pImgDir)
    {
        return getColCount();
    }

    int cy = m_addbkgDlg.height();
    return round(10.0f*cy/__size(2160));

    /*UINT uRet = 9;
    if (cy >= __size(2340))
    {
       uRet++;
    }
    else if (cy <= __size(1920))
    {
        uRet--;
        if (cy < __size(1800))
        {
            uRet = round((float)uRet*cy/__size(1800));
        }
    }
    return uRet;*/
}

size_t CAddBkgView::getItemCount() const
{
    if (m_pImgDir)
    {
        return m_pImgDir->imgCount();
    }

    return m_paImgDirs.size();
}

void CAddBkgView::_onPaintItem(CPainter& painter, tagLVItem& lvItem)
{
    if (m_pImgDir)
    {
        auto pm = m_pImgDir->img(lvItem.uItem);
        if (pm)
        {
            QRect rcFrame(lvItem.rc);
            painter.drawPixmapEx(rcFrame, *pm);

            rcFrame.setLeft(rcFrame.left()-1);
            rcFrame.setTop(rcFrame.top()-1);
            painter.drawRectEx(rcFrame, QColor(255,255,255,50));
        }
    }
    else
    {
        m_paImgDirs.get(lvItem.uItem, [&](IImgDir& imgDir){
            auto eStyle = E_LVItemStyle::IS_MultiLine
                    | E_LVItemStyle::IS_RightTip | E_LVItemStyle::IS_BottomLine;
            tagLVItemContext context(lvItem, eStyle);
            context.strText = imgDir.displayName();
            context.pmIcon = &imgDir.snapshot();
            _paintRow(painter, context);
        });
    }
}

void CAddBkgView::_onRowClick(tagLVItem& lvItem, const QMouseEvent&)
{
    if (m_pImgDir)
    {
        cauto strFilePath = m_pImgDir->imgPath(lvItem.uItem);
        if (!strFilePath.empty())
        {
            m_addbkgDlg.bkgDlg().addBkg(strFilePath);
        }
    }
    else
    {
        _saveScrollRecord(NULL);

        m_paImgDirs.get(lvItem.uItem, [&](IImgDir& imgDir){
            showImgDir(imgDir);
        });

        m_addbkgDlg.relayout();
    }
}

void CAddBkgView::showImgDir(IImgDir& imgDir)
{
    m_pImgDir = &imgDir;
    m_eScrollBar = E_LVScrollBar::LVSB_None;
    update();

    timerutil::setTimerEx(100, [=](){
        if (NULL == m_pImgDir)
        {
            return false;
        }

        if (!m_pImgDir->genSubImgs())
        {
            return false;
        }

        update();

        return true;
    });
}

void CAddBkgView::upward()
{
    reset();

    m_eScrollBar = E_LVScrollBar::LVSB_Left;

    m_pImgDir = NULL;

    scroll(_scrollRecord(NULL));
}
