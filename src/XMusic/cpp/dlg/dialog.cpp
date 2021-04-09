
#include "dialog.h"

#include "xmusic.h"

static list<CDialog*> g_lstDlg;

void CDialog::resetPos()
{
    for (auto pDlg : g_lstDlg)
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
        cauto ptCenter = __app.mainWnd().geometry().center();
        move(ptCenter.x()-width()/2, ptCenter.y()-height()/2);
    }

    auto cx = width();
    auto cy = height();
    m_bHLayout = cx > cy;

    _relayout(cx, cy);
}

#include <QBitmap>

void CDialog::show(cfn_void cbClose)
{
    QWidget *parent = NULL;
    if (!g_lstDlg.empty())
    {
        parent = g_lstDlg.front();
    }
    else
    {
        parent = &__app.mainWnd();
    }
    auto flags = windowFlags();// | Qt::Dialog;
    setParent(parent, flags);

    if (!m_bFullScreen)
    {
        // 主要mac需要
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_NoSystemBackground);
    }

    _setPos();

    g_lstDlg.push_front(this);

    connect(this, &QDialog::finished, [&, cbClose]{
        auto itr = std::find(g_lstDlg.begin(), g_lstDlg.end(), this);
        if (itr != g_lstDlg.end())
        {
            g_lstDlg.erase(itr);
        }

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

/*static QWidget *g_pMask = NULL;
static CDialog *g_pDlg = NULL;

void showMask(cqcr crMask, cfn_void cbClose)
{
    QWidget *parent = &__app.mainWnd();
    if (NULL == g_pMask)
    {
        g_pMask = new QWidget(parent);
    }

    if (!g_lstDlg.empty())
    {
        parent = g_lstDlg.front();
    }

    g_pMask->setParent(parent);
    g_pMask->raise();

    auto flags = windowFlags() & (~Qt::Dialog);
    setParent(g_pMask, flags);

    g_pDlg = this;
}*/

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

QColor _crOffset(cqcr cr, uint8_t uOffset, int alpha=255)
{
    auto r = cr.red() > 127 ? (cr.red()-uOffset) : (cr.red()+uOffset);
    auto g = cr.green() > 127 ? (cr.green()-uOffset) : (cr.green()+uOffset);
    auto b = cr.blue() > 127 ? (cr.blue()-uOffset) : (cr.blue()+uOffset);

    return QColor(r,g,b,alpha);
}

void CDialog::_onPaint(CPainter& painter, cqrc rc)
{
    if (!m_bFullScreen)
    {
        cauto crBorder = _crOffset(bkgColor(), 14);

#if __android
        painter.fillRect(rc, bkgColor());
        painter.drawRectEx(rc, crBorder);
        return;
#endif

        painter.fillRectEx(rc, bkgColor(), __dlgRound);
        painter.drawRectEx(rc, crBorder, 1, Qt::SolidLine, __dlgRound);
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
