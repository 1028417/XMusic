
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
}

bool CDialog::event(QEvent *ev)
{
	switch (ev->type())
	{
    case QEvent::Close:
        _onClose();

        g_setFullScreenDlgs.erase(this);

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
#elif __windows
    case QEvent::WindowActivate:
    {
        CDialog *pDlg = this;
        do {
            ::SetWindowPos((HWND)pDlg->m_parent.winId(), pDlg->hwnd(), 0, 0, 0, 0
                           , SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

            pDlg = dynamic_cast<CDialog*>(&pDlg->m_parent);
        } while (pDlg);
    }

        break;
#endif
	default:
        break;
	}

	return QDialog::event(ev);
}
