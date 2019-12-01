
#include "dialog.h"

#include <QPainter>
#include <QBitmap>

QColor g_crTheme(__defThemeColor);

extern void fixWorkArea(QWidget& wnd);

static CDialog* g_pFrontDlg = NULL;

void CDialog::resetPos()
{
    list<CDialog*> lstDlgs;
    for (auto pDlg = g_pFrontDlg; pDlg; )
    {
        lstDlgs.push_front(pDlg);

        pDlg = dynamic_cast<CDialog*>(pDlg->m_parent);
    }

    for (auto pDlg : lstDlgs)
    {
        pDlg->_setPos();
    }
}

void CDialog::_setPos()
{
    if (m_bFullScreen)
    {
        fixWorkArea(*this);
    }
    else
    {
        cauto ptCenter = m_parent->geometry().center();
        move(ptCenter.x()-width()/2, ptCenter.y()-height()/2);
    }
}

void CDialog::show(bool bFullScreen, const fn_void& cbClose)
{
    g_pFrontDlg = this;

    _setBkgColor(g_crTheme);

    this->setWindowFlags(Qt::Dialog);

    this->setWindowFlags(Qt::FramelessWindowHint);

    m_bFullScreen = bFullScreen;
    if (!m_bFullScreen)
    {
        QBitmap bmp(this->size());
        bmp.fill();
        QPainter painter(&bmp);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        painter.drawRoundedRect(bmp.rect(),15,15);
        setMask(bmp);
    }

    _setPos();

    this->connect(this, &QDialog::finished, [&, cbClose](){
        g_pFrontDlg = dynamic_cast<CDialog*>(m_parent);

        _onClose();

        if (cbClose)
        {
            cbClose();
        }
    });

#if __android || __ios // 移动端exec会露出任务栏
    this->setVisible(true);
#else
    //this->exec();
    this->setModal(true);
    this->setVisible(true);
#endif
}

bool CDialog::event(QEvent *ev)
{
	switch (ev->type())
    {
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
            ::SetWindowPos((HWND)pDlg->m_parent->winId(), pDlg->hwnd(), 0, 0, 0, 0
                           , SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

            pDlg = dynamic_cast<CDialog*>(pDlg->m_parent);
        } while (pDlg);
    }

        break;
#endif
	default:
        break;
	}

	return QDialog::event(ev);
}
