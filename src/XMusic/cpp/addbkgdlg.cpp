#include "app.h"

#include "bkgdlg.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

CAddBkgDlg::CAddBkgDlg(class CApp& app, CBkgDlg& bkgDlg) : CDialog(m_bkgDlg)
    , m_app(app),
    m_bkgDlg(bkgDlg),
    m_addbkgView(app, *this, m_paImgDirs)
{
}

void CAddBkgDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(1.15, E_FontWeight::FW_SemiBold);

#if __android || __ios
    m_addbkgView.setFont(1.05);
#endif

    connect(ui.btnReturn, &CButton::signal_clicked, [&](){
        if (!m_addbkgView.upward())
        {
            close();
        }
    });

    connect(this, &CAddBkgDlg::signal_founddir, this, &CAddBkgDlg::slot_founddir);
}

void CAddBkgDlg::show(IImgDir *pImgDir, cfn_void cbClose)
{
    if (pImgDir)
    {
        m_addbkgView.showImgDir(*pImgDir);
    }

    CDialog::show([=](){
        m_paImgDirs.clear();

        m_addbkgView.clear();

        cbClose();
    });
}

void CAddBkgDlg::_relayout(int cx, int cy)
{
    int sz = cy>cx ? cy/11.1 : cy/6.1;
    int xMargin = sz/4;
    QRect rcReturn(xMargin, xMargin, sz-xMargin*2, sz-xMargin*2);
    if (CApp::checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        rcReturn.moveTop(__cyIPhoneXBangs - rcReturn.top());
    }
    ui.btnReturn->setGeometry(rcReturn);

    ui.labelTitle->move(rcReturn.right() + xMargin, rcReturn.center().y() - ui.labelTitle->height()/2);

    int y_addbkgView = 0;
    if (m_addbkgView.isInRoot())
    {
        y_addbkgView = rcReturn.bottom() + rcReturn.top();
    }
    m_addbkgView.setGeometry(0, y_addbkgView, cx, cy-y_addbkgView);
}

void CAddBkgDlg::addBkg(const wstring& strFile)
{
    m_bkgDlg.addBkg(strFile);
    close();
}


CAddBkgView::CAddBkgView(class CApp& app, CAddBkgDlg& addbkgDlg, const TD_ImgDirList& paImgDir) :
    CListView(&addbkgDlg)
    , m_app(app)
    , m_addbkgDlg(addbkgDlg)
    , m_paImgDirs(paImgDir)
{
}

size_t CAddBkgView::getColumnCount()
{
    if (m_pImgDir)
    {
        if (m_pImgDir->subImgs().size() <= 4)
        {
            return 2;
        }

        return 3;
    }

    return 1;
}

size_t CAddBkgView::getPageRowCount()
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

size_t CAddBkgView::getRowCount()
{
    if (m_pImgDir)
    {
        return (UINT)ceil((float)m_pImgDir->subImgs().size()/getColumnCount());
    }

    return m_paImgDirs.size();
}

void CAddBkgView::_onPaintRow(CPainter& painter, tagLVRow& lvRow)
{
    if (m_pImgDir)
    {
        size_t uIdx = lvRow.uRow * getColumnCount() + lvRow.uCol;

        cauto subImgs = m_pImgDir->subImgs();
        if (uIdx < subImgs.size())
        {
            QRect rcFrame(lvRow.rc);
            cauto pm = subImgs[uIdx].first;
            painter.drawPixmapEx(rcFrame, pm);

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
            context.strText = imgDir.fileName();

            context.pixmap = imgDir.snapshot();

            _paintRow(painter, context);
        });
    }
}

void CAddBkgView::_onRowClick(tagLVRow& lvRow, const QMouseEvent&)
{
    if (m_pImgDir)
    {
        size_t uIdx = lvRow.uRow * getColumnCount() + lvRow.uCol;

        cauto subImgs = m_pImgDir->subImgs();
        if (uIdx < subImgs.size())
        {
            m_addbkgDlg.addBkg(subImgs[uIdx].second);
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
