
#include "dialog.h"

#include "xmusic.h"

static CDialog* g_pFrontDlg = NULL;

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
        extern void fixScreen(QWidget& wnd);
        fixScreen(*this);
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

#include <QBitmap>

void CDialog::_show(cfn_void cbClose)
{
    if (!m_bFullScreen)
    {
        // 主要mac需要
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_NoSystemBackground);
    }

    _setPos();

    g_pFrontDlg = this;

    connect(this, &QDialog::finished, [&, cbClose]{
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

    this->setModal(true); //this->setWindowModality(Qt::ApplicationModal);

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
#if __android
        if (Qt::Key_Back != ((QKeyEvent*)ev)->key())
        {
            break;
        }
#endif

        if (!_handleReturn())
        {
            close(); // 相比5.6.3，5.13.2有bug，必须这样手动close
        }

        return true;
#endif
	default:
        break;
	}

    return TWidget::event(ev);
}

inline static void _crRevert(QColor& cr, int alpha=10)
{
    cr.setRed(255-cr.red());
    cr.setGreen(255-cr.green());
    cr.setBlue(255-cr.blue());
    cr.setAlpha(alpha);
}

void CDialog::_onPaint(CPainter& painter, cqrc rc)
{
    if (!m_bFullScreen)
    {
        auto cr = bkgColor();

#if __android
        painter.fillRect(rc, cr);

        _crRevert(cr);
        painter.drawRect(rc, cr);

        return;
#endif

        painter.fillRectEx(rc, cr, __dlgRound);

        _crRevert(cr);
        painter.drawRectEx(rc, cr, __dlgRound);
    }
    else
    {
        if (!__app.mainWnd().drawBkg(m_bHLayout, painter, rc))
        {
            painter.fillRect(rc, bkgColor());

            /*__app.mainWnd().drawDefaultBkg(painter, rc, 0, 0.1f);
            auto cr = bkgColor();
            cr.setAlpha(205);
            painter.fillRect(rc, cr);*/

            return;
        }

        auto cr = bkgColor();
        cr.setAlpha(246);
        painter.fillRect(rc, cr);
    }
}
