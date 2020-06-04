
#include "dialog.h"

#include <QBitmap>

#include <QStyleOption>

extern void fixWorkArea(QWidget& wnd);

static CDialog* g_pFrontDlg = NULL;

#define __xround 15

void CDialog::resetPos()
{
    list<CDialog*> lstDlgs;
    for (auto pDlg = g_pFrontDlg; pDlg; )
    {
        lstDlgs.push_front(pDlg);

        pDlg = dynamic_cast<CDialog*>(&pDlg->m_parent);
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
        cauto ptCenter = m_parent.geometry().center();
        move(ptCenter.x()-width()/2, ptCenter.y()-height()/2);
    }
}

void CDialog::_show(cfn_void cbClose)
{
    /*if (!m_bFullScreen)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_NoSystemBackground);
    }*/

    _setPos();

    g_pFrontDlg = this;

    connect(this, &QDialog::finished, [&, cbClose]() {
        g_pFrontDlg = dynamic_cast<CDialog*>(&m_parent);

        _onClosed();

        if (cbClose)
        {
            cbClose();
        }

        disconnect(this, &QDialog::finished, 0, 0);
    });

#if __mac
    this->exec();
#else
#if __windows
    this->setModal(true); //this->setWindowModality(Qt::ApplicationModal);
#endif
    this->setVisible(true);
#endif
}

void CDialog::show(cfn_void cbClose)
{
    show(m_parent, cbClose);
}

void CDialog::show(QWidget& parent, cfn_void cbClose)
{
    auto flags = windowFlags() | Qt::Dialog;
    setParent(&parent, flags);

    _show(cbClose);
}

bool CDialog::event(QEvent *ev)
{
	switch (ev->type())
    {
    case QEvent::Move:
    case QEvent::Resize:
    case QEvent::Show:
    {
        auto cx = width();
        auto cy = height();
        m_bHLayout = cx > cy;

        _relayout(cx, cy);
    }

        break;
    case QEvent::Paint:
    {
        CPainter painter(this);
        cauto rc = rect();
        if (!m_bFullScreen)
        {
            extern QColor g_crLogoBkg;
            painter.fillRect(rc, g_crLogoBkg);

            painter.fillRectEx(rc, bkgColor(), __xround);
        }
        else
        {
            painter.fillRect(rc, bkgColor());
        }
    }

        break;
#if __android || __ios
	case QEvent::KeyRelease:
        if (_handleReturn())
        {
            return false;
        }
#endif
	default:
        break;
	}

	return QDialog::event(ev);
}
