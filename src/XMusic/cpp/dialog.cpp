
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

    auto cx = width();
    auto cy = height();
    m_bHLayout = cx > cy;

    _relayout(cx, cy);
}

void CDialog::_show(cfn_void cbClose)
{
    // 主要mac需要
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);

/*#if !__android
    if (!m_bFullScreen)
    {
        QBitmap bmp(this->size());
        bmp.fill();

        CPainter painter(&bmp);
        painter.setPen(Qt::transparent);
        painter.setBrush(Qt::black);
        painter.drawRectEx(bmp.rect(), __xround);
        painter.end();

        this->setMask(bmp);
    }
#endif*/

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

#if __mac // 规避全屏的bug
    this->exec();
    return;
#endif

#if !__android //??安卓好像有问题
    this->setModal(true); //this->setWindowModality(Qt::ApplicationModal);
#endif

    this->setVisible(true);
}

void CDialog::show(cfn_void cbClose)
{
    show(m_parent, cbClose);
}

void CDialog::show(QWidget& parent, cfn_void cbClose)
{
    auto flags = windowFlags();// | Qt::Dialog;
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
#if __android || __ios
	case QEvent::KeyRelease:
        if (!_handleReturn())
        {
            close(); // 相比5.6.3，5.13.2有bug，必须这样手动clode
        }

        return true;
#endif
	default:
        break;
	}

    return TWidget::event(ev);
}

void CDialog::_onPaint(CPainter& painter, cqrc rc)
{
    if (!m_bFullScreen)
    {
    #if __android
        extern QColor g_crLogoBkg;
        painter.fillRect(rc, g_crLogoBkg);
    #endif

        painter.fillRectEx(rc, bkgColor(), __xround);
    }
    else
    {
        painter.fillRect(rc, bkgColor());
    }
}
