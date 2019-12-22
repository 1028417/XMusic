
#include "dialog.h"

#include <QBitmap>

extern void fixWorkArea(QWidget& wnd);

static CDialog* g_pFrontDlg = NULL;

#define __xround 15

void CDialog::resetPos()
{
    list<CDialog*> lstDlgs;
    for (auto pDlg = g_pFrontDlg; pDlg; )
    {
        lstDlgs.push_front(pDlg);

        pDlg = dynamic_cast<CDialog*>(pDlg->parent());
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
        auto pParent = dynamic_cast<QWidget*>(parent());
        if (pParent)
        {
            cauto ptCenter = pParent->geometry().center();
            move(ptCenter.x()-width()/2, ptCenter.y()-height()/2);
        }
    }
}

void CDialog::show(QWidget& parent, bool bFullScreen, const fn_void& cbClose)
{
    setParent(&parent, Qt::Dialog | Qt::FramelessWindowHint);

    m_bFullScreen = bFullScreen;
    if (!m_bFullScreen)
    {
        setAttribute(Qt::WA_TranslucentBackground); //setAttribute(Qt::WA_NoSystemBackground);

        QBitmap bmp(this->size());
        bmp.fill();

        CPainter painter(&bmp);
        //painter.setPen(Qt::transparent);
        painter.setBrush(Qt::black);
        painter.drawRectEx(bmp.rect(), __xround);

        this->setMask(bmp);
    }

    _setPos();

    g_pFrontDlg = this;
    connect(this, &QDialog::finished, [&, cbClose]() {
        g_pFrontDlg = dynamic_cast<CDialog*>(&parent);

        _onClose();

        if (cbClose)
        {
            cbClose();
        }
    });

#if __android || __ios // 移动端exec会露出任务栏
    this->setVisible(true);
#elif __mac
    this->exec();
#else
    this->setModal(true); //this->setWindowModality(Qt::ApplicationModal);
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
    case QEvent::Paint:
        if (m_bFullScreen)
        {
            QPainter painter(this);
            painter.fillRect(rect(), bkgColor());
        }
        else
        {
            CPainter painter(this);
            painter.fillRectEx(rect(), bkgColor(), __xround);
        }

        break;
#if __android || __ios
	case QEvent::KeyRelease:
		if (!_handleReturn())
		{
			close();
		}

        return true;
#endif
/*#if __windows
    case QEvent::WindowActivate:
    {
        CDialog *pDlg = this;
        do {
            auto pParent = dynamic_cast<QWidget*>(pDlg->parent());

            ::SetWindowPos((HWND)pParent->winId(), pDlg->hwnd(), 0, 0, 0, 0
                           , SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

            pDlg = dynamic_cast<CDialog*>(pParent);
        } while (pDlg);
    }

        break;
#endif*/
	default:
        break;
	}

	return QDialog::event(ev);
}
