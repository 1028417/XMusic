
#include "label.h"

#include "xmusic.h"

void CLabel::_onPaint(CPainter& painter, cqrc rc)
{
    if (m_br)
    {
        QRect rcSrc(0,0,m_br.width(), m_br.height());

        /*cauto qsText = this->text();
        if (!qsText.isEmpty())
        {
            painter.drawBrushEx(rc, m_br, rcSrc, m_szRound);
            _paintText(painter, rc);
            return;
        }*/

        painter.drawBrushEx(rc, m_br, rcSrc, m_szRound);
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


void CLabelButton::_onPaint(CPainter& painter, cqrc rc)
{
    if (m_bPressing)
    {
        painter.setOpacity(0.5f);
    }

    CLabel::_onPaint(painter, rc);
}

void CLabelButton::_onMouseEvent(E_MouseEventType type, const QMouseEvent& me)
{
    if (E_MouseEventType::MET_Press == type)
    {
        m_bPressing = true;
        update();
    }
    else if (E_MouseEventType::MET_Release == type)
    {
        UINT uDelayTime = 100;
#if __windows || __mac
        uDelayTime = 200;
#endif
        async(uDelayTime, [&]{
            m_bPressing = false;
            update();
        });
    }

    CLabel::_onMouseEvent(type, me);
}


void CMovieLabel::show(bool bShow)
{
    if (bShow)
    {
        if (NULL == this->movie())
        {
            this->raise();

            m_movie.setFileName(m_qsMovie);
            this->setMovie(&m_movie);
        }
        this->movie()->start();
    }
    else
    {
        auto movie = this->movie();
        if (movie)
        {
            movie->stop();
        }
    }
    QLabel::setVisible(bShow);
}

CLoadingLabel::CLoadingLabel(QWidget *parent) : CMovieLabel(":/img/loading.gif", parent)
{
    this->resize(__size(400), __size(330));

    this->setStyleSheet("QWidget{background-color:rgb(255, 225, 31, 100); \
                                   border-top-left-radius:10px; border-top-right-radius:10px; \
                                   border-bottom-left-radius:10px; border-bottom-right-radius:10px;}");
}
