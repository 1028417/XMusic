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

    connect(ui.btnReturn, &CButton::signal_clicked, [&](){
        if (!m_lv.upward())
        {
            close();
        }
    });
}

void CAddBkgDlg::show(IImgDir *pImgDir, cfn_void cbClose)
{
    if (pImgDir)
    {
        m_lv.showImgDir(*pImgDir);
    }

    CDialog::show([=](){
        if (cbClose)
        {
            cbClose();
        }

        m_lv.reset();
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
    }
    m_lv.setGeometry(0, y_addbkgView, cx, cy-y_addbkgView);

#if !__windows
    m_lv.setVisible(false);
    CApp::async([&](){
        m_lv.setVisible(true);
    });
#endif
}

CAddBkgView::CAddBkgView(CAddBkgDlg& addbkgDlg, const TD_ImgDirList& paImgDir) :
    CListView(addbkgDlg)
    , m_addbkgDlg(addbkgDlg)
    , m_paImgDirs(paImgDir)
{
}

size_t CAddBkgView::getColumnCount() const
{
    if (m_pImgDir)
    {
        if (m_pImgDir->imgCount() <= 4)
        {
            return 2;
        }

        return 3;
    }

    return 1;
}

size_t CAddBkgView::getPageRowCount() const
{
    if (m_pImgDir)
    {
        return getColumnCount();
    }

    UINT uRet = 8;
    int cy = m_addbkgDlg.height();
    if (cy > __size(2160))
    {
        uRet++;
        if (cy >= __size(2560))
        {
           uRet++;
        }
    }
    else if (cy < __size(1800))
    {
        uRet--;
        uRet = ceil((float)uRet*m_addbkgDlg.height()/__size(1800));
    }
    return uRet;
}

size_t CAddBkgView::getRowCount() const
{
    if (m_pImgDir)
    {
        return (UINT)ceil((float)m_pImgDir->imgCount()/getColumnCount());
    }

    return m_paImgDirs.size();
}

void CAddBkgView::_onPaintRow(CPainter& painter, tagLVRow& lvRow)
{
    if (m_pImgDir)
    {
        size_t uIdx = lvRow.uRow * getColumnCount() + lvRow.uCol;
        auto pm = m_pImgDir->img(uIdx);
        if (pm)
        {
            QRect rcFrame(lvRow.rc);
            painter.drawPixmapEx(rcFrame, *pm);

            rcFrame.setLeft(rcFrame.left()-1);
            rcFrame.setTop(rcFrame.top()-1);
            painter.drawRectEx(rcFrame, QColor(255,255,255,50));
        }
    }
    else
    {
        m_paImgDirs.get(lvRow.uRow, [&](IImgDir& imgDir){
            auto eStyle = E_RowStyle::IS_MultiLine
                    | E_RowStyle::IS_RightTip | E_RowStyle::IS_BottomLine;
            tagRowContext context(lvRow, eStyle);
            context.strText = imgDir.displayName();
            context.pmImg = &imgDir.snapshot();
            _paintRow(painter, context);
        });
    }
}

void CAddBkgView::_onRowClick(tagLVRow& lvRow, const QMouseEvent&)
{
    if (m_pImgDir)
    {
        size_t uIdx = lvRow.uRow * getColumnCount() + lvRow.uCol;

        cauto strFilePath = m_pImgDir->imgPath(uIdx);
        if (!strFilePath.empty())
        {
            m_addbkgDlg.bkgDlg().addBkg(strFilePath);
        }
    }
    else
    {
        _saveScrollRecord(NULL);

        m_paImgDirs.get(lvRow.uRow, [&](IImgDir& imgDir){
            showImgDir(imgDir);
        });

        m_addbkgDlg.relayout();
    }
}

void CAddBkgView::showImgDir(IImgDir& imgDir)
{
    m_pImgDir = &imgDir;
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

bool CAddBkgView::upward()
{
    if (m_pImgDir)
    {
        m_pImgDir = NULL;
        reset();

        scroll(_scrollRecord(NULL));

        if (!m_paImgDirs)
        {
            return false;
        }

        m_addbkgDlg.relayout();

        return true;
    }

    return false;
}
