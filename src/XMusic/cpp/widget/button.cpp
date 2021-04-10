
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
        CPainter painter(this, QPainter::Antialiasing | QPainter::TextAntialiasing);
        QColor cr = foreColor();

        cr.setAlpha(13);
        auto szRound = height()/3;///2;
        painter.fillRectEx(rc, cr, szRound);

        cr.setAlpha(CPainter::oppTextAlpha(130));
        painter.drawRectEx(rc, cr, 1, Qt::PenStyle::SolidLine, szRound);
        painter.drawTextEx(rc, Qt::AlignCenter|Qt::AlignVCenter, qsText, cr);
    }
    else
    {
        if (!m_bPressing)
        {
            //unsetOpacityEffect();
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
        //unsetDropShadowEffect();
        setOpacityEffect(0.5);
	}
	else if (E_MouseEventType::MET_Release == type)
    {
        //m_bPressing = true;
        //unsetDropShadowEffect();
        //setOpacityEffect(0.5);

		UINT uDelayTime = 100;
#if __windows || __mac
		uDelayTime = 200;
#endif
        async(uDelayTime, [&]{
            unsetOpacityEffect();
            m_bPressing = false;
        });
	}
	else if (E_MouseEventType::MET_Click == type)
	{
		emit signal_clicked(this);
	}
}
