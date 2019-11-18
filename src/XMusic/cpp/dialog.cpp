
#include "dialog.h"

extern void showFull(QWidget* wnd);

extern std::set<class CDialog*> g_setFullScreenDlgs;

void CDialog::show(bool bFullScreen)
{
    _setBkgColor();

    this->setWindowFlags(Qt::Dialog);
    this->setWindowFlags(Qt::FramelessWindowHint);

    if (bFullScreen)
    {
        showFull(this);
        g_setFullScreenDlgs.insert(this);
    }

    this->exec();

/*#if __mac
    this->exec();

#elif __windows
    //this->setModal(true); //this->setWindowModality(Qt::WindowModal);

    this->setWindowModality(Qt::ApplicationModal);
    this->setVisible(true);

#else
    this->setVisible(true);
#endif*/
}

bool CDialog::event(QEvent *ev)
{
	switch (ev->type())
	{
    case QEvent::Close:
        _onClose();

        g_setFullScreenDlgs.erase(this);

#if __windows
        m_parent.activateWindow();
#endif

		break;
	case QEvent::Move:
	case QEvent::Resize:
        _relayout(width(), height());

		break;
#if __android || __ios
	case QEvent::KeyRelease:
		if (!_handleReturn())
		{
			close();
		}

		return true;

		break;
#endif
	default:
		break;
	}

	return QDialog::event(ev);
}
