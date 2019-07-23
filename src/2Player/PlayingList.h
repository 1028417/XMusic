
#pragma once

#include "widget.cpp"

#include "view.h"

struct tagPlayingItem
{
    wstring strTitle;

    int nDuration = -1;

    wstring strSinger;
    wstring strAlbum;

    wstring strPlaylist;
};

#include <QWidget>
class CPlayingList : public CWidget<QWidget>
{
public:
    CPlayingList(class CPlayerView& view, QWidget *parent=NULL) :
        CWidget(parent, {Qt::TapAndHoldGesture})
        , m_view(view)
    {
        this->startTimer(1000);
    }

private:
    class CPlayerView& m_view;

    PairList<UINT, QString> m_plPlayingItems;

    UINT m_uPlayingItem = 0;

    QFont m_font;
    QColor m_crText;

    UINT m_uMinRowHeight = 100;
    UINT m_uRowHeight = 0;

    UINT m_uItemCount = 0;

    UINT m_uMaxScrollPos = 0;
    float m_fScrollPos = 0;

    bool m_bHittestPlayingItem = false;

    int m_nActiveTime = 0;

public:
    void setMinRowHeight(UINT uMinRowHeight)
    {
        m_uMinRowHeight = uMinRowHeight;
    }

    void setFont(const QFont& font, const QColor& crText)
    {
        m_font = font;
        m_crText = crText;
    }

    void updateList(PairList<UINT, QString>& m_plPlayingItems, UINT uPlayingItem);

    void updatePlayingItem(UINT uPlayingItem, bool bHittestPlayingItem);

private:
    void _onMouseEnter() override;
    void _onMouseLeave() override;

    void _onTouchBegin(const QPointF&) override;
    void _onTouchEnd() override;
    void _onTouchMove(int dy) override;

    void _onGesture(QGesture&) override;

    void mouseDoubleClickEvent(QMouseEvent *ev) override;

    void _onPaint(QPainter& painter, const QRect& rcPos) override;
    void _onPaintItem(QPainter& painter, UINT uItem, UINT uID
                      , const QString& qsTitle, QRect& rcItem);

    void _updateActive(int nActiveTime=6);

    void timerEvent(QTimerEvent *);
};
