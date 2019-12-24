
#include "button.h"

#include "App.h"

bool CButton::event(QEvent *ev)
{
    if (ev->type() == QEvent::Paint)
    {
        setDropShadowEx();
	}

	return CWidget::event(ev);
}

void CButton::_onMouseEvent(E_MouseEventType type, const QMouseEvent&)
{
	if (E_MouseEventType::MET_Press == type)
	{
        setOpacityEffect(0.5);
	}
	else if (E_MouseEventType::MET_Release == type)
	{
        setOpacityEffect(0.5);

		UINT uDelayTime = 100;
#if __windows || __mac
		uDelayTime = 200;
#endif
        CApp::async(uDelayTime, [&](){
			unsetOpacityEffect();
        });
	}
	else if (E_MouseEventType::MET_Click == type)
	{
		emit signal_clicked(this);
	}
}
