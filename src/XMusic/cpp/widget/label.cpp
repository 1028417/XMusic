
#include "label.h"

void CLabel::_onPaint(CPainter& painter, cqrc rc)
{
    //cauto rc = this->rect();
    if (m_br)
    {
        painter.drawBrushEx(rc, m_br, QRect(0,0,m_br.width(), m_br.height()), m_szRound);
        if (m_uShadowWidth > 0)
        {
            for (UINT uIdx=0; uIdx<m_uShadowWidth; uIdx++)
            {
                UINT uAlpha = m_uShadowAlpha * (m_uShadowWidth-uIdx)/m_uShadowWidth;

                QRect rcShadow(uIdx, uIdx, rc.right()-uIdx*2, rc.bottom()-uIdx*2);
                painter.drawRectEx(rcShadow, __ShadowColor(uAlpha), m_szRound);
            }
        }
        // 提速 return;
	}

    _paintText(painter, rc);
}

void CLabel::_paintText(CPainter& painter, cqrc rc)
{
    QString qsText = this->text();
    if (!qsText.isEmpty())
    {
        int flag = this->alignment();

        int cx = rc.width();
        if (-1 == m_flag)
		{
            QFont font = painter.font();

#if (QT_VERSION >= QT_VERSION_CHECK(5,13,0))
#define __checkTextWidth(t) painter.fontMetrics().horizontalAdvance(t)
#else
#define __checkTextWidth(t) painter.fontMetrics().width(t)
#endif
            while (__checkTextWidth(qsText) >= cx)
			{
				auto fPointSize = font.pointSizeF()-0.1;
				if (fPointSize < 0)
				{
					break;
				}

				font.setPointSizeF(fPointSize);
				painter.setFont(font);
			}
		}
		else
        {
            //qsText = painter.fontMetrics().elidedText(qsText, Qt::ElideRight, cx);
            flag |= m_flag;
        }

        /*auto crFore = foreColor();
        if (!this->isEnabled())
        {
            crFore.setAlpha(crFore.alpha()/2);
        }*/

        m_rcText = painter.drawTextEx(rc, flag, qsText, foreColor(), m_uShadowWidth, m_uShadowAlpha);
    }
}

void CLabel::_onMouseEvent(E_MouseEventType type, const QMouseEvent& me)
{
	if (E_MouseEventType::MET_Click == type)
    {
        if (!text().isEmpty() && !m_rcText.contains(me.pos()))
		{
            return;
        }

        emit signal_clicked(this, me.pos());
	}
}
