
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
    }

public:
    void performClick(int x=0, int y=0)
    {
        emit signal_clicked(this, {x,y});
    }

    void onClicked(cfn_void fn)
    {
        //connect(this, &CLabel::signal_clicked, fn);
        onUISignal(&CLabel::signal_clicked, fn);
    }

    template <typename _slot>
    void onClicked(TD_XObj<_slot> recv, _slot slot)
    {
        onUISignal(&CLabel::signal_clicked, recv, slot);
    }

private:
    CBrush m_br;

    QColor m_crText;
    int m_flag = -1;

    UINT m_uShadowWidth = 1;
    UINT m_uShadowAlpha = __ShadowAlpha;

    UINT m_szRound = 0;

    QRect m_rcClickable;

signals:
    void signal_clicked(CLabel*, const QPoint&);

protected:
    virtual void _onPaint(CPainter& painter, cqrc rc) override;

    virtual cqrc _paintText(CPainter& painter, cqrc rc, cqstr qsText);

    virtual void _onMouseEvent(E_MouseEventType type, const QMouseEvent& me) override;

    virtual bool _checkClickable(cqpt);

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

    void setPixmap(const CBrush& br, UINT szRound=(UINT)-1)
    {
        m_br = br;
        if (szRound != (UINT)-1)
        {
            m_szRound = szRound;
        }
    }

    void setPixmap(cqpm pm, UINT szRound=(UINT)-1)
    {
        m_br.setTexture(pm);
        if (szRound != (UINT)-1)
        {
            m_szRound = szRound;
        }
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


#if !__android
class CTipLabel : public CLabel
{
public:
    CTipLabel();

private:
    cqrc _paintText(CPainter& painter, cqrc rc, cqstr qsText) override;

public:
    void show(QWidget& parent, cqstr qtTip, UINT uShowTime=0);
};
#endif
