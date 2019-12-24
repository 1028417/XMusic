
#include "button.h"

#include "App.h"

bool CButton::event(QEvent *ev)
{
    if (!m_bPressing && ev->type() == QEvent::Paint)
    {
        unsetOpacityEffect();
        setDropShadowEx();
    }

	return CWidget::event(ev);
}

void CButton::_onMouseEvent(E_MouseEventType type, const QMouseEvent&)
{
	if (E_MouseEventType::MET_Press == type)
	{
        m_bPressing = true;
        unsetDropShadowEffect();
        setOpacityEffect(0.5);
	}
	else if (E_MouseEventType::MET_Release == type)
    {
        m_bPressing = true;
        unsetDropShadowEffect();
        setOpacityEffect(0.5);

		UINT uDelayTime = 100;
#if __windows || __mac
		uDelayTime = 200;
#endif
        CApp::async(uDelayTime, [&](){
            unsetOpacityEffect();
            m_bPressing = false;
        });
	}
	else if (E_MouseEventType::MET_Click == type)
	{
		emit signal_clicked(this);
	}
}
