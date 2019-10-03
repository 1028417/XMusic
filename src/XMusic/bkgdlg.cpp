#include "app.h"

#include "bkgdlg.h"
#include "ui_bkgdlg.h"

#include "mainwindow.h"

#define __margin    __size(40)

#define __xsize     __size(60)

static Ui::BkgDlg ui;

CBkgView::CBkgView(class CXMusicApp& app, CBkgDlg& bkgDlg)
    : CListView(&bkgDlg),
    m_app(app),
    m_bkgDlg(bkgDlg)
{
    (void)m_pmX.load(":/img/btnX.png");
}

size_t CBkgView::getPageRowCount()
{
    if (m_bkgDlg.bkgCount() <= 3)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

inline size_t CBkgView::getColumnCount()
{
    if (m_bkgDlg.bkgCount() <= 3)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

size_t CBkgView::getRowCount()
{
    return (UINT)ceil((1.0+m_bkgDlg.bkgCount())/ getColumnCount());
}

void CBkgView::_onPaintRow(CPainter& painter, const tagLVRow& lvRow)
{
    size_t uColumnCount = getColumnCount();
    int nMargin = __margin/(uColumnCount-1);

    QRect rc = lvRow.rc;
    int cy = rc.height()-nMargin;
    int cx = 0;

    if (2 == uColumnCount)
    {
        cx = cy*m_bkgDlg.width()/m_bkgDlg.height();
        cx = MIN(cx, rc.width()-nMargin/2);
    }
    else
    {
        cx = rc.width()-nMargin;
    }

    if (0 == lvRow.uCol)
    {
        rc.setRect(rc.right()-nMargin/2-cx, rc.top(), cx, cy);
    }
    else
    {
        rc.setRect(rc.left()+nMargin/2, rc.top(), cx, cy);
    }

    UINT uItem = lvRow.uRow * uColumnCount + lvRow.uCol;
    if (0 == uItem)
    {
        m_app.mainWnd().drawDefaultBkg(painter, rc);
    }
    else
    {
        UINT uIdx = uItem-1;
        cauto pm = m_bkgDlg.snapshot(uIdx);
        if (pm)
        {
            painter.drawPixmapEx(rc, *pm);

            painter.drawPixmap(rc.right()-__xsize, rc.top(), __xsize, __xsize, m_pmX);
        }
        else
        {
            QPixmap pmAdd;
            if (pmAdd.load(":/img/add.png"))
            {
#define __offset __size(60)
                painter.drawPixmap(rc.center().x()-__offset, rc.center().y()-__offset, __offset*2, __offset*2, pmAdd);
            }

            painter.drawFrame(8, rc, 255,255,255,255, Qt::BrushStyle::Dense7Pattern);

            return;
        }
    }

    painter.drawFrame(1, rc, 255,255,255,128);
}

void CBkgView::_onRowClick(const tagLVRow& lvRow, const QMouseEvent& me)
{
    size_t uIdx = lvRow.uRow * getColumnCount() + lvRow.uCol;

    if (uIdx != 0 && uIdx < m_bkgDlg.bkgCount())
    {
        if (me.pos().x() >= lvRow.rc.right()-__xsize && me.pos().y() <= lvRow.rc.top()+__xsize)
        {
            m_bkgDlg.deleleBkg(uIdx);
            return;
        }
    }

    m_bkgDlg.setBkg(uIdx);
}

CBkgDlg::CBkgDlg(class CXMusicApp& app) :
    m_app(app),
    m_addbkgDlg(app, *this),
    m_bkgView(app, *this)
{
}

void CBkgDlg::init()
{
    ui.setupUi(this);

    QColor crText(32, 128, 255);
    ui.labelTitle->setTextColor(crText);
    ui.labelTitle->setFont(1.15, E_FontWeight::FW_SemiBold);

    connect(ui.btnReturn, &CButton::signal_clicked, [&](CButton*) {
        this->close();
    });

#define __hbkgdir L"/hbkg/"
#define __vbkgdir L"/vbkg/"

    m_strHBkgDir = fsutil::workDir() + __hbkgdir;
    m_strVBkgDir = fsutil::workDir() + __vbkgdir;

    auto strFileName = to_wstring(time(NULL));

    if (!fsutil::existDir(m_strHBkgDir))
    {
        if (fsutil::createDir(m_strHBkgDir))
        {
            fsutil::copyFile(L"assets:" __hbkgdir L"win10" , m_strHBkgDir + strFileName);
        }
    }

    if (!fsutil::existDir(m_strVBkgDir))
    {
        if (fsutil::createDir(m_strVBkgDir))
        {
            fsutil::copyFile(L"assets:" __vbkgdir L"win10", m_strVBkgDir + strFileName);
        }
    }

    fsutil::findSubFile(m_strHBkgDir, [&](const wstring& strSubFile) {
        m_vecHBkg.push_back(strSubFile);
    });

    fsutil::findSubFile(m_strVBkgDir, [&](const wstring& strSubFile) {
        m_vecVBkg.push_back(strSubFile);
    });

    cauto& strHBkg = m_app.getOptionMgr().getOption().strHBkg;
    if (!strHBkg.empty())
    {
        (void)m_pmHBkg.load(m_strHBkgDir + strHBkg);
    }

    cauto& strVBkg = m_app.getOptionMgr().getOption().strVBkg;
    if (!strVBkg.empty())
    {
        (void)m_pmVBkg.load(m_strVBkgDir + strVBkg);
    }

    m_addbkgDlg.init();
}

void CBkgDlg::_relayout(int cx, int cy)
{
    static const QRect rcReturnPrev = ui.btnReturn->geometry();
    QRect rcReturn = __rect(rcReturnPrev);
    if (cy >= __size(812*3)) // 针对全面屏刘海作偏移
    {
#define __yOffset __size(66)
        rcReturn.setTop(rcReturn.top() + __yOffset);
        rcReturn.setBottom(rcReturn.bottom() + __yOffset);
    }
    ui.btnReturn->setGeometry(rcReturn);

    m_bHScreen = cx>cy;
    if (m_bHScreen)
    {
        int x_bkgView = rcReturn.right()+__margin/2;

        m_bkgView.setGeometry(x_bkgView, __margin, cx-x_bkgView-__margin, cy-__margin-__margin/2);

        ui.labelTitle->move(cx, rcReturn.center().y() - ui.labelTitle->height()/2);
    }
    else
    {
        int y_bkgView = rcReturn.bottom() + rcReturn.top();

        int cy_bkgView = cy-y_bkgView;
        int cx_bkgView = cy_bkgView*cx/cy;
        m_bkgView.setGeometry((cx-cx_bkgView)/2, y_bkgView, cx_bkgView, cy_bkgView);

        ui.labelTitle->move((cx-ui.labelTitle->width())/2, rcReturn.center().y() - ui.labelTitle->height()/2);
    }
}

const QPixmap* CBkgDlg::snapshot(UINT uIdx)
{
    auto& vecSnapshot = m_bHScreen?m_vecHSnapshot:m_vecVSnapshot;
    if (uIdx < vecSnapshot.size())
    {
        return vecSnapshot[uIdx];
    }

    auto& vecBkg = m_bHScreen?m_vecHBkg:m_vecVBkg;
    if (uIdx >= vecBkg.size())
    {
        return NULL;
    }

    cauto& stBkgDir = m_bHScreen?m_strHBkgDir:m_strVBkgDir;
    cauto& strBkgFile = stBkgDir + vecBkg[uIdx];

#define __zoomoutSize 1000
    m_lstSnapshot.push_back(QPixmap());
    auto pm = &m_lstSnapshot.back();
    if (pm->load(strBkgFile))
    {
        CPainter::zoomoutPixmap(*pm, __zoomoutSize);
    }

    vecSnapshot.push_back(pm);
    return pm;
}

void CBkgDlg::_setBkg(const wstring& strBkg)
{
    if (m_bHScreen)
    {
        m_app.getOptionMgr().getOption().strHBkg = strBkg;
    }
    else
    {
        m_app.getOptionMgr().getOption().strVBkg = strBkg;
    }

    QPixmap& pmBkg = m_bHScreen? m_pmHBkg:m_pmVBkg;
    if (!strBkg.empty())
    {
        cauto& stBkgDir = m_bHScreen?m_strHBkgDir:m_strVBkgDir;
        (void)pmBkg.load(stBkgDir + strBkg);
    }
    else
    {
        pmBkg = QPixmap();
    }

    m_app.mainWnd().updateBkg();
}

void CBkgDlg::setBkg(size_t uIdx)
{
    auto& vecBkg = m_bHScreen?m_vecHBkg:m_vecVBkg;
    if (0 == uIdx)
    {
        _setBkg(L"");
        close();
    }
    else
    {
        uIdx--;
        if (uIdx < vecBkg.size())
        {
            cauto& strBkg = vecBkg[uIdx];
            _setBkg(strBkg);
            close();
        }
        else
        {
#if __windows
#define __MediaFilter L"所有支持图片|*.Jpg;*.Jpeg;*.Png;*.Gif;*.Bmp|Jpg文件(*.Jpg)|*.Jpg|Jpeg文件(*.Jpeg)|*.Jpeg \
                |Png文件(*.Png)|*.Png|Gif文件(*.Gif)|*.Gif|位图文件(*.Bmp)|*.Bmp|"
            tagFileDlgOpt FileDlgOpt;
            FileDlgOpt.strTitle = L"选择背景图";
            FileDlgOpt.strFilter = __MediaFilter;
            FileDlgOpt.hWndOwner = (HWND)this->winId();
            CFileDlg fileDlg(FileDlgOpt);

            wstring strFile = fileDlg.ShowOpenSingle();
            if (!strFile.empty())
            {
                this->addBkg(strFile);
            }
#else
            m_addbkgDlg.show();
#endif
        }
    }
}

void CBkgDlg::addBkg(const wstring& strFile)
{
    wstring strNewName = to_wstring(time(NULL));

    auto& stBkgDir = m_bHScreen?m_strHBkgDir:m_strVBkgDir;
    if (fsutil::copyFile(strFile, stBkgDir + strNewName))
    {
        auto& vecBkg = m_bHScreen?m_vecHBkg:m_vecVBkg;
        vecBkg.push_back(strNewName);

        setBkg(vecBkg.size());
    }
}

void CBkgDlg::deleleBkg(size_t uIdx)
{
    if (0 == uIdx)
    {
        return;
    }
    uIdx--;

    auto& vecBkg = m_bHScreen?m_vecHBkg:m_vecVBkg;
    if (uIdx < vecBkg.size())
    {
        auto& strBkg = m_bHScreen?m_app.getOptionMgr().getOption().strHBkg
                                  :m_app.getOptionMgr().getOption().strVBkg;
        if (strBkg == vecBkg[uIdx])
        {
            _setBkg(L"");
        }

        auto& stBkgDir = m_bHScreen?m_strHBkgDir:m_strVBkgDir;
        fsutil::removeFile(stBkgDir + vecBkg[uIdx]);

        vecBkg.erase(vecBkg.begin()+uIdx);

        auto& vecSnapshot = m_bHScreen?m_vecHSnapshot:m_vecVSnapshot;
        if (uIdx < vecSnapshot.size())
        {
            vecSnapshot.erase(vecSnapshot.begin()+uIdx);
        }

        m_bkgView.update();
    }
}

void CBkgDlg::_onClose()
{
    if (m_addbkgDlg.isVisible())
    {
        m_addbkgDlg.close();
    }

    m_vecHSnapshot.clear();
    m_vecVSnapshot.clear();
    m_lstSnapshot.clear();

    m_bkgView.reset();
}
