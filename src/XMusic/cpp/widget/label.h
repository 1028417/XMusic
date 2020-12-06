
#pragma once

#include <QLabel>

#include "widget.h"

#include <QTextOption>

class CBrush
{
public:
    CBrush() = default;

private:
    QBrush m_br;
    int m_cx = 0;
    int m_cy = 0;

public:
    operator bool() const
    {
        return m_cx > 0;
    }
    bool operator !() const
    {
        return 0 == m_cx;
    }

    cqbr brush() const
    {
        return m_br;
    }
    QBrush& brush()
    {
        return m_br;
    }

    operator cqbr() const
    {
        return m_br;
    }
    operator QBrush&()
    {
        return m_br;
    }

    int width() const
    {
        return m_cx;
    }
    int height() const
    {
        return m_cy;
    }

    void setTexture(cqpm pm)
    {
        m_cx = pm.width();
        if (0 == m_cx)
        {
            clear();
            return;
        }
        m_cy = pm.height();

        m_br.setTexture(pm);
    }

    void setTexture(cqstr qsFile)
    {
        setTexture(QPixmap(qsFile));
    }

    void clear()
    {
        QBrush br;
        m_br.swap(br);
        m_cx = m_cy = 0;
    }
};

enum class E_LabelTextOption
{
    LTO_AutoFit,
    LtO_Elided
};

class CLabel : public TWidget<QLabel>
{
    Q_OBJECT
public:
    CLabel(QWidget *parent) : TWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    CBrush m_br;

    QColor m_crText;
    int m_flag = -1;

    bool m_bUseCustomColor = false;

    UINT m_uShadowWidth = 1;
    UINT m_uShadowAlpha = __ShadowAlpha;

    UINT m_szRound = 0;

    QRect m_rcText;

signals:
    void signal_click(CLabel*, const QPoint& pos);

private:
    void _onPaint(CPainter& painter, cqrc rc) override;

    void _onMouseEvent(E_MouseEventType type, const QMouseEvent& me) override;

public:
    bool pixmap() const
    {
        return m_br.width() > 0;
    }

    int pixmapWidth() const
    {
        return m_br.width();
    }
    int pixmapHeight() const
    {
        return m_br.height();
    }

    void setPixmap(const CBrush& br)
    {
        m_br = br;
    }
    void setPixmap(cqpm pm)
    {
        m_br.setTexture(pm);
    }

    void setPixmapRound(UINT szRound)
    {
        m_szRound = szRound;
    }

    void setText(const QString &qsText, int flag = -1)
    {
        m_flag = flag;
        QLabel::setText(qsText);
    }

    void setShadow(UINT uWidth, UINT uAlpha=__ShadowAlpha)
    {
        m_uShadowWidth = uWidth;
        m_uShadowAlpha = uAlpha;
        update();
    }

    void clear()
    {
        QLabel::clear();

        m_br.clear();
    }
};
