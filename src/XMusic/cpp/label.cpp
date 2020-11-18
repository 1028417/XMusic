
#include "label.h"

void CLabel::_onPaint(CPainter& painter, cqrc)
{
	m_rc = this->rect();

    auto pm = QLabel::pixmap();
	if (pm && !pm->isNull())
    {
        painter.drawPixmapEx(m_rc, *pm, m_szRound);

        if (m_uShadowWidth > 0)
        {
            for (UINT uIdx=0; uIdx<m_uShadowWidth; uIdx++)
            {
                UINT uAlpha = m_uShadowAlpha * (m_uShadowWidth-uIdx)/m_uShadowWidth;
                painter.setPen(__ShadowColor(uAlpha));

                QRect rcShadow(uIdx, uIdx, m_rc.right()-uIdx*2, m_rc.bottom()-uIdx*2);
                painter.drawRectEx(rcShadow, m_szRound);
            }
        }

		return;
	}

    QString qsText = this->text();
    if (!qsText.isEmpty())
    {
        int flag = this->alignment();

        int cx = m_rc.width();
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

        m_rc = painter.drawTextEx(m_rc, flag, qsText, foreColor(), m_uShadowWidth, m_uShadowAlpha);
    }
}

void CLabel::_onMouseEvent(E_MouseEventType type, const QMouseEvent& me)
{
	if (E_MouseEventType::MET_Click == type)
	{
        if (m_rc.contains(me.pos()))
		{
			emit signal_click(this, me.pos());
		}
	}
}
