
#include "dialog.h"

#include "xmusic.h"

static list<CDialog*> g_lstDlg;

CDialog* CDialog::frontDlg()
{
    if (g_lstDlg.empty())
    {
        return NULL;
    }
    return g_lstDlg.front();
}

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
}

void CDialog::connect_dlgClose(CButton *btn)
{
    btn->connect_dlgClose(this);
}

void CDialog::show(cfn_void cbClose)
{
    QWidget *parent = frontDlg();
    if (NULL == parent)
    {
        parent = &__app.mainWnd();
    }
    setParent(parent, windowFlags());

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

//#if !__android
    this->setModal(true); //this->setWindowModality(Qt::ApplicationModal);
//#endif

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

#define __dlgAlpha 246

void CDialog::_onPaint(CPainter& painter, cqrc rc)
{
    auto cr = bkgColor();
    if (__app.mainWnd().drawBkg(m_bHLayout, painter, rc))
    {
        cr.setAlpha(__dlgAlpha);
    }
    painter.fillRect(rc, cr);
}

void CMaskDlg::showMask(cqcr crMask, cfn_void cbClose)
{
    m_crMask = crMask;

    auto prevFlags = m_child.windowFlags();
    auto flags = prevFlags & (~(Qt::Dialog | Qt::Window));
    m_child.setParent(this, flags);

    m_child.onUISignal(&QDialog::finished, [&]{
        this->close();
    });

/*#if __android
    this->setWindowFlags(flags);
#endif*/
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
    m_bInit = true;

    if (NULL == m_pDlgMask)
    {
        m_pDlgMask = new CMaskDlg(*this);
    }

    m_pDlgMask->showMask(crMask, cbClose);
}

void CDialogEx::show(cfn_void cbClose)
{
    m_bInit = true;

#if __android
    CDialog::show(cbClose);
    return;
#endif

    show(QColor(0,0,0,85), cbClose);
}

void CDialogEx::_setPos()
{
    cauto rc = m_pDlgMask?m_pDlgMask->rect():__app.mainWnd().geometry();
    m_bHLayout = rc.width() > rc.height();

    cauto ptCenter = rc.center();
    move(ptCenter.x()-width()/2, ptCenter.y()-height()/2);
    this->update();//for安卓横屏竖屏切换
}

void CDialogEx::_onPaint(CPainter& painter, cqrc rc)
{
#if __android
    UINT uround = 0;
#else
    UINT uround = __dlgRound;
#endif

    CPainterClipGuard guard(painter, rc, uround);

    cauto mainWnd = __app.mainWnd();
    QRect rcDst = mainWnd.rect();
    QPoint ptOffset;
    if (NULL == m_pDlgMask)
    {
        ptOffset = mainWnd.pos();
    }
    rcDst.moveLeft(ptOffset.x()-this->x());
    rcDst.moveTop(ptOffset.y()-this->y());
    if (mainWnd.drawBkg(m_bHLayout, painter, rcDst))
    {
        painter.fillRect(rc, bkgColor(__dlgAlpha));
        return;
    }

    cauto cr = bkgColor();
    painter.fillRect(rc, cr);

    if (NULL == m_pDlgMask)
    {
        guard.restore();
        cauto crBorder = _crOffset(cr, 14);
        painter.drawRectEx(rc, crBorder, uround);
    }
}
