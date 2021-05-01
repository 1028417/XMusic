
#include "xmusic.h"

#include "button.h"

void CButton::connect_dlgClose(CDialog *dlg)
{
    onClicked([=]{
        dlg->close();
    });
}

void CButton::_onPaint(CPainter& painter, cqrc rc)
{
    cauto qsText = this->text();
    if (!qsText.isEmpty())
    {
        QColor cr = foreColor();

        cr.setAlpha(13);
        auto szRound = height()/3;///2;
        painter.fillRectEx(rc, cr, szRound);

        cr.setAlpha(CPainter::oppTextAlpha(130));
        painter.drawRectEx(rc, cr, szRound);
        painter.drawTextEx(rc, Qt::AlignCenter|Qt::AlignVCenter, qsText, cr);
    }
    else
    {
        if (!m_bPressing)
        {
            this->setDropShadowEffect(__ShadowColor(__ShadowAlpha), 1, 1);
        }
        TWidget::_onPaint(painter, rc);
    }
}

void CButton::_onMouseEvent(E_MouseEventType type, const QMouseEvent&)
{
	if (E_MouseEventType::MET_Press == type)
	{
        m_bPressing = true;
        setOpacityEffect(0.5);
	}
	else if (E_MouseEventType::MET_Release == type)
    {
#if __windows || __mac
#define __delayTime 150
#else
#define __delayTime 100
#endif
        async(__delayTime, [&]{
            m_bPressing = false;
            unsetOpacityEffect();
        });
	}
	else if (E_MouseEventType::MET_Click == type)
	{
		emit signal_clicked(this);
	}
}
