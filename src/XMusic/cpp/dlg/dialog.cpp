
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
    setGeometry(__app.mainWnd().geometry());

    /*extern void fixScreen(QWidget& wnd);
    fixScreen(*this);

    auto cx = width();
    auto cy = height();
    m_bHLayout = cx > cy;

    _relayout(cx, cy);*/
}

void CDialog::connect_dlgClose(CButton *btn)
{
    btn->connect_dlgClose(this);
}

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
    setParent(parent, windowFlags()); //| Qt::Dialog);

    _setPos();

    g_lstDlg.push_front(this);

    onUISignal(&QDialog::finished, [&, cbClose]{
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
        if (Qt::Key_Back != ((QKeyEvent*)ev)->key()) // TODO ios
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

void CMaskDlg::showMask(cqcr crMask, cfn_void cbClose)
{
    m_crMask = crMask;

    auto prevFlags = m_child.windowFlags();
    auto flags = prevFlags & (~(Qt::Dialog |Qt::WindowCloseButtonHint |Qt::WindowSystemMenuHint));
    m_child.setParent(this, flags);

    m_child.onUISignal(&QDialog::finished, [&]{
        this->close();
    });

    CDialog::show([=]{
        if (cbClose)
        {
            cbClose();
        }
        m_child.setParent(NULL, prevFlags);
    });
}

void CMaskDlg::_onPaint(CPainter& painter, cqrc rc)
{
    painter.fillRect(rc, m_crMask);
}

void CDialogEx::show(cqcr crMask, cfn_void cbClose)
{
    if (NULL == m_pDlgMask)
    {
        m_pDlgMask = new CMaskDlg(*this);
    }

    m_pDlgMask->showMask(crMask, cbClose);
}

void CDialogEx::show(cfn_void cbClose)
{
#if __android
    CDialog::show(cbClose);
    return;
#endif

    show(QColor(0,0,0,64), cbClose);
}

void CDialogEx::_setPos()
{
    cauto ptCenter = (m_pDlgMask?m_pDlgMask->rect():__app.mainWnd().geometry()).center();
    move(ptCenter.x()-width()/2, ptCenter.y()-height()/2);
}

void CDialogEx::_onPaint(CPainter& painter, cqrc rc)
{
    cauto cr = bkgColor();
    cauto crBorder = _crOffset(cr, 14);

#if __android
    painter.fillRect(rc, cr);
    painter.drawRectEx(rc, crBorder);
    return;
#endif

    painter.fillRectEx(rc, cr, __dlgRound);
    painter.drawRectEx(rc, crBorder, __dlgRound);
}
