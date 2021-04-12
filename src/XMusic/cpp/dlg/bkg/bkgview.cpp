
#include "xmusic.h"

#include "bkgview.h"

#define __ceilCount ((m_bkgDlg.bkgCount() <= 2) ? 2 : 3)

CBkgView::CBkgView(CBkgDlg& bkgDlg)
    : CListView(&bkgDlg)
    , m_bkgDlg(bkgDlg)
{
}

UINT CBkgView::margin()
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
            auto br = m_bkgDlg.thumbsBrush(lvItem.uItem-2);
            if (br)
            {
                painter.drawImgEx(rc, *br, QRect(0,0,br->m_cx,br->m_cy), __szRound);

                QRect rcX(rc.right()-m_szButton, rc.top(), m_szButton, m_szButton);
                painter.drawImg(rcX, m_pmX);

                /*或改为滚动停止处理static UINT s_uSequence = 0;
                s_uSequence++;
                if (lvItem.uCol == uColCount-1)
                {
                    auto uPageRowCount = getRowCount();
                    UINT uFloorRow = ceil(scrollPos()+uPageRowCount-1);
                    if (lvItem.uRow == uFloorRow)
                    {
                        auto uSequence = s_uSequence;
                        async(300, [=]{
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
                }*/

                return;
            }

            if (lvItem.uItem < 9)
            {
                return;
            }
        }

        rc.setLeft(rc.left()+2);
        rc.setRight(rc.right()-1);

        extern QColor _crOffset(cqcr cr, uint8_t uOffset, int alpha);
        auto cr = _crOffset(g_crBkg, 25, 255);
        painter.drawRectEx(rc, cr, Qt::PenStyle::DotLine, 2, __szRound);

        //cr.setAlpha(200);

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
            if (me.pos().x() > lvItem.rc.width()-m_szButton && me.pos().y() < m_szButton)
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
