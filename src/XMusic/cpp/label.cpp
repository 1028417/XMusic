
#include "label.h"

void CLabel::_onPaint(CPainter& painter, const QRect& rc)
{
	m_rc = this->rect();

	auto pm = pixmap();
	if (pm && !pm->isNull())
	{
#define __szRound 8
        QRect rcImg(1, 1, m_rc.width()-2, m_rc.height()-2);
        painter.drawPixmapEx(rcImg, *pm, __szRound);

        if (m_uShadowWidth > 0 && m_uShadowAlpha > 0)
        {
            m_crShadow.setAlpha(m_uShadowAlpha);
            for (UINT uIdx=0; uIdx<m_uShadowWidth; uIdx++)
            {
                m_crShadow.setAlpha(m_uShadowAlpha*(m_uShadowWidth-uIdx)/m_uShadowWidth);
                painter.setPen(m_crShadow);

                QRect rcShadow(uIdx, uIdx, m_rc.right()-uIdx*2, m_rc.bottom()-uIdx*2);
                painter.drawRectEx(rcShadow, __szRound);
            }
        }

		return;
	}

    painter.setPen(g_crText);

	QString text = this->text();
    if (!text.isEmpty())
    {
		int cx = m_rc.right();

		if (E_LabelTextOption::LTO_AutoFit == m_eTextOption)
		{
            QFont font = painter.font();

#if (QT_VERSION >= QT_VERSION_CHECK(5,13,0))
#define __checkTextWidth(t) painter.fontMetrics().horizontalAdvance(t)
#else
#define __checkTextWidth(t) painter.fontMetrics().width(t)
#endif
            while (__checkTextWidth(text) >= cx)
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
			text = painter.fontMetrics().elidedText(text, Qt::ElideRight, cx, Qt::TextShowMnemonic);
		}

		auto alignment = this->alignment();

        if (m_uShadowWidth > 0 && m_uShadowAlpha > 0)
        {
            QPen pen = painter.pen();

            m_crShadow.setAlpha(m_uShadowAlpha);
            for (UINT uIdx=0; uIdx<=m_uShadowWidth; uIdx++)
            {
                if (uIdx > 1)
                {
                    m_crShadow.setAlpha(m_uShadowAlpha*(m_uShadowWidth-uIdx+1)/m_uShadowWidth);
                }

                painter.setPen(m_crShadow);

                painter.drawText(QRectF(uIdx, uIdx, cx, m_rc.bottom()), alignment, text);
            }

            painter.setPen(pen);
        }

		painter.drawText(m_rc, alignment, text, &m_rc);

		return;
	}

	CWidget<QLabel>::_onPaint(painter, rc);
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
