
#include "xmusic.h"

#include "bkgdlg.h"
#include "ui_bkgdlg.h"

#define __snapshotRetain 6

#define __ceilCount ((m_bkgDlg.bkgCount() <= 2) ? 2 : 3)

static int g_xsize = 0;

static Ui::BkgDlg ui;

CBkgView::CBkgView(CBkgDlg& bkgDlg)
    : CListView(&bkgDlg)
    , m_bkgDlg(bkgDlg)
{
}

inline UINT CBkgView::margin()
{
#define __margin __size(40)
    return __margin/(__ceilCount-1);
}

inline size_t CBkgView::getRowCount() const
{
    return __ceilCount;
}

inline size_t CBkgView::getColCount() const
{
    return __ceilCount;
}

size_t CBkgView::getItemCount() const
{
    auto uItemCount = 2+m_bkgDlg.bkgCount();
    auto uColCount = getColCount();
    auto nMod = uItemCount%uColCount;
    if (0 == nMod)
    {
        uItemCount += uColCount;
    }
    else
    {
        uItemCount += (uColCount-nMod);
    }

    return uItemCount;
}

void CBkgView::_onPaintItem(CPainter& painter, tagLVItem& lvItem)
{
    size_t uColCount = getColCount();

    int nMargin = margin();

    QRect rc = lvItem.rc;
    if (rc.width() > rc.height())
    {
        nMargin /= 2;
        rc.setLeft(rc.left() + nMargin);
        rc.setTop(rc.top() + nMargin);
        rc.setRight(rc.right() - nMargin);
        rc.setBottom(rc.bottom() - nMargin);
    }
    else
    {
        int cy = rc.height()-nMargin;
        int cx = 0;

        if (2 == uColCount)
        {
            cx = cy*m_bkgDlg.width()/m_bkgDlg.height();
            cx = MIN(cx, rc.width()-nMargin/2);
        }
        else
        {
            cx = rc.width()-nMargin;
        }

        if (0 == lvItem.uCol)
        {
            rc.setRect(rc.right()-nMargin/2-cx, rc.top(), cx, cy);
        }
        else
        {
            rc.setRect(rc.left()+nMargin/2, rc.top(), cx, cy);
        }
    }

    if (1 == lvItem.uItem)
    {
        __app.mainWnd().drawDefaultBkg(painter, rc, __szRound);
    }
    else
    {
        if (lvItem.uItem >= 2)
        {
            auto br = m_bkgDlg.brush(lvItem.uItem-2);
            if (br)
            {
                painter.drawPixmapEx(rc, *br, QRect(0,0,br->m_cx,br->m_cy), __szRound);

                QRect rcX(rc.right()-g_xsize-5, rc.top()+5, g_xsize, g_xsize);
                painter.drawPixmap(rcX, m_pmX);

                static UINT s_uSequence = 0;
                s_uSequence++;
                if (lvItem.uCol == uColCount-1)
                {
                    auto uPageRowCount = getRowCount();
                    UINT uFloorRow = ceil(scrollPos()+uPageRowCount-1);
                    if (lvItem.uRow == uFloorRow)
                    {
                        auto uSequence = s_uSequence;
                        async(300, [=](){
                            if (uSequence != s_uSequence || !isVisible())
                            {
                                return;
                            }

                            for (UINT uCol=0; uCol<uColCount; uCol++)
                            {
                                auto uItem = (uFloorRow+1) * uColCount + uCol;
                                if (uItem >= 2)
                                {
                                    m_bkgDlg.brush(uItem-2);
                                }
                            }
                        });
                    }
                }

                return;
            }

            if (lvItem.uItem < 9)
            {
                return;
            }
        }

        rc.setLeft(rc.left()+2);
        rc.setRight(rc.right()-1);

        /*int r = g_crBkg.red();
        int g = g_crBkg.green();
        int b = g_crBkg.blue();
        QColor cr(r<128?r+128:r-128, g<128?g+128:g-128, b<128?b+128:b-128);
        int d = abs(cr.red()+cr.green()+cr.blue()-g_crBkg.red()-g_crBkg.green()-g_crBkg.blue());
        if (abs(d) == 128)
        {
            cr.setAlpha(100);
        }
        else
        {
            cr.setAlpha(50);
        }*/
        auto cr = g_crFore;
        cr.setAlpha(100);

        painter.drawRectEx(rc, cr, 2, Qt::PenStyle::DotLine, __szRound);

        cr.setAlpha(128);

        int len = MIN(rc.width(), rc.height())/4;
#define __szAdd __size(4)
        cauto pt = rc.center();
        rc.setRect(pt.x()-len/2, pt.y()-__szAdd/2, len, __szAdd);
        painter.fillRectEx(rc, cr, __szAdd/2);

        rc.setRect(pt.x()-__szAdd/2, pt.y()-len/2, __szAdd, len);
        painter.fillRect(rc, g_crBkg);
        painter.fillRectEx(rc, cr, __szAdd/2);
    }
}

void CBkgView::_onItemClick(tagLVItem& lvItem, const QMouseEvent& me)
{
    if (lvItem.uItem >= 2)
    {
        auto uIdx = lvItem.uItem-2;
        if (uIdx < m_bkgDlg.bkgCount())
        {
            if (me.pos().x() > lvItem.rc.width()-g_xsize && me.pos().y() < g_xsize)
            {
                m_bkgDlg.deleleBkg(uIdx);
                return;
            }
        }
        else
        {
            if (lvItem.uItem<9)
            {
                return;
            }
        }
    }

    m_bkgDlg.handleLVClick(lvItem.uItem);
}

CBkgDlg::CBkgDlg(QWidget& parent) : CDialog(parent),
    m_option(__app.getOption()),
    m_lv(*this),
    m_addbkgDlg(*this),
    m_colorDlg(*this)
{
}

void CBkgDlg::init()
{
    m_colorDlg.init();
    m_addbkgDlg.init();

    m_lv.init();

    ui.setupUi(this);

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnColor, &CButton::signal_clicked, [&]() {
        m_colorDlg.show();
    });
}

void zoomoutPixmap(QPixmap& pm, int cx, int cy)
{
    if (pm.isNull())
    {
        return;
    }

    if ((float)pm.height()/pm.width() > (float)cy/cx)
    {
        if (pm.width() > cx)
        {
            auto&& temp = pm.scaledToWidth(cx, Qt::SmoothTransformation);
            pm.swap(temp);
        }
    }
    else
    {
        if (pm.height() > cy)
        {
            auto&& temp = pm.scaledToHeight(cy, Qt::SmoothTransformation);
            pm.swap(temp);
        }
    }
}

static void zoomoutPixmap(bool bHLayout, QPixmap& pm, float fDiv=0.f)
{
    int cx = 0;
    int cy = 0;
    if (bHLayout)
    {
        cx = g_screen.szScreenMax;
        cy = g_screen.szScreenMin;
    }
    else
    {
        cx = g_screen.szScreenMin;
        cy = g_screen.szScreenMax;
    }

    if (fDiv > 1)
    {
        cx /= fDiv;
        cy /= fDiv;
    }

    zoomoutPixmap(pm, cx, cy);
}

inline CBkgBrush& CBkgDlg::_addbr(QPixmap& pm, bool bHLayout)
{
    zoomoutPixmap(bHLayout, pm, 2.5f);
    m_lstBr.emplace_back(pm);
    return m_lstBr.back();
}

void CBkgDlg::preinitBkg(bool bHLayout)
{
    auto& strBkgDir = bHLayout?m_strHBkgDir:m_strVBkgDir;
    strBkgDir = g_strWorkDir + (bHLayout?L"/hbkg/":L"/vbkg/");
    wstring strAppBkgDir = strBkgDir + __app.appVersion();
    if (!fsutil::existDir(strAppBkgDir))
    {
        (void)fsutil::createDir(strBkgDir);

        strAppBkgDir.push_back('-');
        (void)fsutil::createDir(strAppBkgDir);

#if __android
        wstring strBkgSrc = L"assets:";
#else
        wstring strBkgSrc = __app.applicationDirPath().toStdWString();
#endif
        strBkgSrc.append(L"/bkg/");

        QPixmap pm;
        for (wchar_t wch = L'a'; wch <= L'z'; wch++)
        {
            auto strBkg = strBkgSrc + wch + L".jpg";
            auto strDstFile = strAppBkgDir + L"/0." + wch;
            if (pm.load(__WS2Q(strBkg)))
            {
                zoomoutPixmap(bHLayout, pm);
                pm.save(__WS2Q(strDstFile), "JPG", 89);
                mtutil::usleep(10);
            }

            strBkg = strBkgSrc + (bHLayout?L"hbkg/":L"vbkg/") + wch + L".jpg";
            strDstFile = strAppBkgDir + L"/1." + wch;
            fsutil::copyFile(strBkg, strDstFile);

            strBkg = strBkgSrc + (bHLayout?L"hbkg/city/":L"vbkg/city/") + wch + L".jpg";
            strDstFile = strAppBkgDir + L"/2." + wch;
            fsutil::copyFile(strBkg, strDstFile);

            if (!g_bRunSignal)
            {
                return;
            }
        }

        auto t_strAppBkgDir = strAppBkgDir;
        strAppBkgDir.pop_back();
        (void)fsutil::moveFile(t_strAppBkgDir, strAppBkgDir);
    }

    auto& vecBkgFile = bHLayout?m_vecHBkgFile:m_vecVBkgFile;
    fsutil::findSubFile(strAppBkgDir, [&](cwstr strSubFile) {
        vecBkgFile.emplace_back(false, __app.appVersion() + __wcPathSeparator + strSubFile);
    });

    fsutil::findFile(strBkgDir, [&](const tagFileInfo& fileInfo) {
        cauto strPath = strBkgDir+fileInfo.strName;
        if (fileInfo.bDir)
        {
            if (fileInfo.strName != __app.appVersion())
            {
                (void)fsutil::removeDirTree(strPath);
            }
        }
        else
        {
            vecBkgFile.insert(vecBkgFile.begin(), tagBkgFile(false, fileInfo.strName));
        }
    });

    for (auto itr = vecBkgFile.begin(); itr != vecBkgFile.end()
         && itr-vecBkgFile.begin() < __snapshotRetain; ++itr)
    {
        QPixmap pm(strBkgDir + itr->strFile);
        itr->br = &_addbr(pm, bHLayout);

        mtutil::usleep(10);

        if (!g_bRunSignal)
        {
            return;
        }
    }

    cauto strBkg = bHLayout?m_option.strHBkg:m_option.strVBkg;
    if (!strBkg.empty())
    {
        (bHLayout?m_pmHBkg:m_pmVBkg).load(__WS2Q(strBkgDir + strBkg));
    }
}

size_t CBkgDlg::caleRowCount(int cy)
{
    /*UINT uRowCount = 9;
    if (cy >= __size(2340))
    {
       uRowCount++;
    }
    else if (cy <= __size(1920))
    {
        uRowCount--;
        if (cy < __size(1800))
        {
            uRowCount = round((float)uRowCount*cy/__size(1800));
        }
    }*/

    return round(10.0f*cy/__size(2160));
}

void CBkgDlg::_relayout(int cx, int cy)
{
    int sz = MAX(cx, cy)/(CBkgDlg::caleRowCount(MAX(cx, cy))+1.6f);
    int cxMargin = sz/4;
    QRect rcReturn(cxMargin, cxMargin, sz-cxMargin*2, sz-cxMargin*2);
    if (checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        rcReturn.moveTop(__cyIPhoneXBangs - rcReturn.top());
    }
    ui.btnReturn->setGeometry(rcReturn);

    auto szBtn = rcReturn.width();
    g_xsize = szBtn-__size(5);

    if (m_bHLayout)
    {
        ui.btnColor->setGeometry(cxMargin, cy - rcReturn.top() - szBtn, szBtn, szBtn);

        int x_bkgView = rcReturn.right()+cxMargin;
        int cx_bkgView = cx-x_bkgView;
        int cy_bkgView = cx_bkgView*cy/cx;
        m_lv.setGeometry(x_bkgView - m_lv.margin()/2
                              , (cy-cy_bkgView)/2, cx_bkgView, cy_bkgView);

        ui.labelTitle->setGeometry(0, 0, x_bkgView, cy);
        ui.labelTitle->setWordWrap(true);
        ui.labelTitle->setText("设\n置\n背\n景");
    }
    else
    {
        ui.btnColor->setGeometry(cx-cxMargin-szBtn, rcReturn.top(), szBtn, szBtn);

        int y_bkgView = rcReturn.bottom() + rcReturn.top();
        int cy_bkgView = cy-y_bkgView;
        int cx_bkgView = cy_bkgView*cx/cy;
        m_lv.setGeometry((cx-cx_bkgView)/2, y_bkgView, cx_bkgView, cy_bkgView);

        ui.labelTitle->setGeometry(0, 0, cx, y_bkgView);
        ui.labelTitle->setText("设置背景");
    }

    static BOOL bHLayout = -1;
    if (bHLayout != (BOOL)m_bHLayout)
    {
        bHLayout = m_bHLayout;
        m_lv.scroll(0);
    }
}

size_t CBkgDlg::bkgCount() const
{
    return (m_bHLayout?m_vecHBkgFile:m_vecVBkgFile).size();
}

CBkgBrush* CBkgDlg::brush(size_t uIdx)
{
    auto& vecBkgFile = _vecBkgFile();
    if (uIdx >= vecBkgFile.size())
    {
        return NULL;
    }

    auto& bkgFile = vecBkgFile[uIdx];
    if (bkgFile.br)
    {
        return bkgFile.br;
    }

    QPixmap pm(_bkgDir() + bkgFile.strFile);
    auto& br = _addbr(pm, m_bHLayout);
    return bkgFile.br = &br;
}

bool CBkgDlg::_setBkg(int nIdx)
{
    auto& vecBkgFile = _vecBkgFile();
    if (nIdx >= (int)vecBkgFile.size())
    {
        return false;
    }

    auto& pm = m_bHLayout?m_pmHBkg:m_pmVBkg;
    if (nIdx >= 0)
    {
        cauto strFile = vecBkgFile[nIdx].strFile;
        (void)pm.load(_bkgDir() + strFile);
        _updateBkg(strFile);
    }
    else
    {
        pm = QPixmap();
        _updateBkg(L"");
    }

    return true;
}

void CBkgDlg::_updateBkg(cwstr strFile)
{
    m_option.bUseBkgColor = false;
    if (m_bHLayout)
    {
        m_option.strHBkg = strFile;
    }
    else
    {
        m_option.strVBkg = strFile;
    }

    __app.mainWnd().updateBkg();
}

void CBkgDlg::switchBkg(bool bHLayout, bool bNext)
{
    m_bHLayout = bHLayout;
    cauto vecBkgFile = _vecBkgFile();

    cauto strFile = bHLayout ? m_option.strHBkg : m_option.strVBkg;
    int nIdx = -1;
    if (!strFile.empty())
    {
        for (UINT uIdx = 0; uIdx < vecBkgFile.size(); uIdx++)
        {
            if (vecBkgFile[uIdx].strFile == strFile)
            {
                nIdx = uIdx;
                break;
            }
        }
    }

    if (!m_option.bUseBkgColor)
    {
        if (bNext)
        {
            nIdx++;
            if (nIdx >= (int)vecBkgFile.size())
            {
                nIdx = -1;
            }
        }
        else
        {
            if (-1 == nIdx)
            {
                nIdx = vecBkgFile.size()-1;
            }
            else
            {
                nIdx--;
            }
        }
    }

    _setBkg(nIdx);
}

void CBkgDlg::handleLVClick(size_t uItem)
{
    if (0 == uItem)
    {
        m_addbkgDlg.show();
    }
    else if (1 == uItem)
    {
        _setBkg(-1);
        close();
    }
    else
    {
        uItem-=2;
        if (_setBkg(uItem))
        {
            close();
        }
        else
        {
            m_addbkgDlg.show();
        }
    }
}

void CBkgDlg::deleleBkg(size_t uIdx)
{
    auto& vecBkgFile = _vecBkgFile();
    if (uIdx < vecBkgFile.size())
    {
        cauto bkgFile = vecBkgFile[uIdx];

        cauto strBkg = m_bHLayout ? m_option.strHBkg : m_option.strVBkg;
        if (strBkg == bkgFile.strFile)
        {
            _setBkg(-1);
        }

        if (bkgFile.br)
        {
            for (auto itr = m_lstBr.begin(); itr != m_lstBr.end(); ++itr)
            {
                if (&*itr == bkgFile.br)
                {
                    m_lstBr.erase(itr);
                    break;
                }
            }
        }

        fsutil::removeFile(_bkgDir() + bkgFile.strFile);
        vecBkgFile.erase(vecBkgFile.begin()+uIdx);

        m_lv.update();
    }
}

void CBkgDlg::addBkg(cwstr strFile)
{
    auto& pm = m_bHLayout?m_pmHBkg:m_pmVBkg;
    (void)pm.load(__WS2Q(strFile));
    zoomoutPixmap(m_bHLayout, pm);

    cauto strFileName = to_wstring(time(0));
    cauto strDstFile = _bkgDir() + strFileName;
    pm.save(__WS2Q(strDstFile), "JPG", 100);

    auto& vecBkgFile = _vecBkgFile();
    vecBkgFile.insert(vecBkgFile.begin(), tagBkgFile(false, strFileName, &_addbr(pm, m_bHLayout)));
    //update();

    _updateBkg(strFileName);
}

void CBkgDlg::_onClosed()
{
    set<CBkgBrush*> setDeleleBrush;
    if (m_vecHBkgFile.size() > __snapshotRetain)
    {
        for (auto itr = m_vecHBkgFile.begin()+__snapshotRetain; itr != m_vecHBkgFile.end(); ++itr)
        {
            setDeleleBrush.insert(itr->br);
            itr->br = NULL;
        }
    }

    if (m_vecVBkgFile.size() > __snapshotRetain)
    {
        for (auto itr = m_vecVBkgFile.begin()+__snapshotRetain; itr != m_vecVBkgFile.end(); ++itr)
        {
            setDeleleBrush.insert(itr->br);
            itr->br = NULL;
        }
    }

    for (auto itr = m_lstBr.begin(); itr != m_lstBr.end(); )
    {
        if (setDeleleBrush.find(&*itr) != setDeleleBrush.end())
        {
           itr = m_lstBr.erase(itr);
        }
        else
        {
            ++itr;
        }
    }

    m_lv.reset();
}
