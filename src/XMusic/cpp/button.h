
#pragma once

#include <QPushButton>

#include "widget.h"

class CButton : public CWidget<QPushButton>
{
    Q_OBJECT
public:
    CButton(QWidget *parent) : CWidget(parent)
    {
        setFocusPolicy(Qt::FocusPolicy::NoFocus);
    }

signals:
    void signal_clicked(CButton*);

    //void signal_contextMenu(CButton*);

private:
    void _onMouseEvent(E_MouseEventType type, const QMouseEvent&) override
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
            timerutil::async(uDelayTime, [&](){
                setOpacityEffect(1);
            });
        }
        else if (E_MouseEventType::MET_Click == type)
        {
            emit signal_clicked(this);
        }
    }

    /*bool _onGesture(QGesture&) override
    {
        emit signal_contextMenu(this);
        return true;
    }*/
};
