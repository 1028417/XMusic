
#pragma once

#include <QLabel>

#include "widget.h"

#include <QTextOption>

#include <QMovie>

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

public:
    void performClick()
    {
        emit signal_clicked(this, {0,0});
    }

    template <typename _slot>
    void onClicked(TD_XObj<_slot> recv, _slot slot)
    {
        onUISignal(&CLabel::signal_clicked, recv, slot);
    }

    void onClicked(cfn_void fn)
    {
        //connect(this, &CLabel::signal_clicked, fn);
        onUISignal(&CLabel::signal_clicked, fn);
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
    void signal_clicked(CLabel*, const QPoint& pos);

private:
    void _paintText(CPainter& painter, cqrc rc);

protected:
    virtual void _onPaint(CPainter& painter, cqrc rc) override;

    virtual void _onMouseEvent(E_MouseEventType type, const QMouseEvent& me) override;

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
    void setPixmap(const CBrush& br, UINT szRound)
    {
        m_br = br;
        m_szRound = szRound;
    }

    void setPixmap(cqpm pm)
    {
        m_br.setTexture(pm);
    }
    void setPixmap(cqpm pm, UINT szRound)
    {
        m_br.setTexture(pm);
        m_szRound = szRound;
    }

    void setPixmapRound(UINT szRound)
    {
        m_szRound = szRound;
    }

    void clearPixmap()
    {
        m_br = CBrush();
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

class CLabelButton : public CLabel
{
    Q_OBJECT
public:
    CLabelButton(QWidget *parent) : CLabel(parent)
    {
    }

private:
    bool m_bPressing = false;

private:
    void _onPaint(CPainter& painter, cqrc rc) override;

    void _onMouseEvent(E_MouseEventType type, const QMouseEvent& me) override;
};

class CMovieLabel : public QLabel
{
public:
    CMovieLabel(cqstr qsMovie, QWidget *parent=NULL) : QLabel(parent)
      , m_qsMovie(qsMovie)
    {
    }

private:
    QString m_qsMovie;
    QMovie m_movie;

public:
    void show(bool bShow=true);

    void setVisible(bool bVisible=true)
    {
        show(bVisible);
    }
};

class CLoadingLabel : public CMovieLabel
{
public:
    CLoadingLabel(QWidget *parent=NULL);
};
