
#include "dialog.h"

extern void showFull(QWidget* wnd);

extern std::set<class CDialog*> g_setDlgs;

void CDialog::show()
{
    _setBkgColor();

    this->setWindowFlags(Qt::Dialog);
    this->setWindowFlags(Qt::FramelessWindowHint);

    showFull(this);
    g_setDlgs.insert(this);

#if __mac
    this->exec();
#else
#if __windows
    this->setModal(true); //this->setWindowModality(Qt::WindowModal);
#endif

    this->setVisible(true);
#endif
}

bool CDialog::event(QEvent *ev)
{
	switch (ev->type())
	{
    case QEvent::Close:
        _onClose();

        g_setDlgs.erase(this);

#if __windows
        m_parent.activateWindow();
#endif

		break;
	case QEvent::Move:
	case QEvent::Resize:
	{
		int cx = this->width();
		int cy = this->height();
		_relayout(cx, cy);
	}

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
