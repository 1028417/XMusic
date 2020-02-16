
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

void CDialog::show(cfn_void cbClose)
{
    setParent(&m_parent, Qt::Dialog | Qt::FramelessWindowHint);

    _show(cbClose);
}

void CDialog::_show(cfn_void cbClose)
{
    if (!m_bFullScreen)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_NoSystemBackground);

        QBitmap bmp(this->size());
        bmp.fill();

        CPainter painter(&bmp);
        painter.setPen(Qt::transparent);
        painter.setBrush(Qt::black);
        painter.drawRectEx(bmp.rect(), __xround);
        painter.end();

        this->setMask(bmp);
    }

    _setPos();

    g_pFrontDlg = this;
    connect(this, &QDialog::finished, [&, cbClose]() {
        g_pFrontDlg = dynamic_cast<CDialog*>(&m_parent);

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

void CDialog::show(QWidget& parent, cfn_void cbClose)
{
    setParent(&parent, Qt::Dialog | Qt::FramelessWindowHint);

    _show(cbClose);
}

bool CDialog::event(QEvent *ev)
{
	switch (ev->type())
    {
    case QEvent::Move:
    case QEvent::Resize:
        _relayout(width(), height());

        break;
    case QEvent::Show:
        _relayout(width(), height());

        break;
    case QEvent::Paint:
    {
        QPainter painter(this);
        painter.fillRect(rect(), bkgColor());
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
