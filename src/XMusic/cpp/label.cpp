
#include "label.h"

void CLabel::_onPaint(CPainter& painter, const QRect& rc)
{
	m_rc = this->rect();

	auto pm = pixmap();
	if (pm && !pm->isNull())
	{
		painter.drawPixmapEx(m_rc, *pm);
		return;
	}

	QString text = this->text();
	if (!text.isEmpty() && E_LabelTextOption::LTO_None != m_eTextOption)
	{
		int cx = m_rc.right();

		if (E_LabelTextOption::LTO_AutoFit == m_eTextOption)
		{
			QFont font = painter.font();
			while (painter.fontMetrics().width(text) >= cx)
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

		if (0 != m_uShadowWidth)
		{
			QPen pen = painter.pen();

            UINT uShadowAlpha = (UINT)m_crText.alpha();
			if (uShadowAlpha < 255)
			{
				uShadowAlpha /= 2;
			}

			QColor crShadow = m_crShadow;
            crShadow.setAlpha((int)uShadowAlpha);
			painter.setPen(crShadow);

			for (UINT uIdx=0; uIdx<=m_uShadowWidth; uIdx++)
			{
				if (uIdx > 1)
				{
					crShadow.setAlpha(uShadowAlpha*(m_uShadowWidth-uIdx+1)/m_uShadowWidth);
					painter.setPen(crShadow);
				}

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
