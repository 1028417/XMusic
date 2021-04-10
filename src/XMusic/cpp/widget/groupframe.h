
#pragma once

#include <QGroupBox>

#include "widget.cpp"

enum class E_GroupTitlePos
{
    GTP_Top = 0,
    GTP_Bottom,
    GTP_Left,
    GTP_Right,
};

class CGroupFrame : public TWidget<QGroupBox>
{
    Q_OBJECT
public:
    CGroupFrame(QWidget *parent)
        : TWidget(parent, QPainter::Antialiasing | QPainter::TextAntialiasing)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    E_GroupTitlePos m_eTitlePos = E_GroupTitlePos::GTP_Top;

public:
    void setTitle(cqstr qsTitle, E_GroupTitlePos ePos = E_GroupTitlePos::GTP_Top)
    {
        QGroupBox::setTitle(qsTitle);
        m_eTitlePos = ePos;
        update();
    }

private:
    void _onPaint(CPainter& painter, cqrc) override
    {
        auto rc = rect();

        int nAlign = 0;
        if (E_GroupTitlePos::GTP_Bottom == m_eTitlePos)
        {
            nAlign = Qt::AlignBottom | Qt::AlignHCenter;
        }
        else if (E_GroupTitlePos::GTP_Left == m_eTitlePos)
        {
            nAlign = Qt::AlignLeft | Qt::AlignVCenter;
        }
        else if (E_GroupTitlePos::GTP_Right == m_eTitlePos)
        {
            nAlign = Qt::AlignRight | Qt::AlignVCenter;
        }
        else
        {
            nAlign = Qt::AlignTop | Qt::AlignHCenter;
        }

        auto rcTitle = painter.drawTextEx(rc, nAlign, title());

        int titleMargin = __size(18);
        cauto ptCenter = rcTitle.center();
        if (E_GroupTitlePos::GTP_Bottom == m_eTitlePos)
        {
            rcTitle.setLeft(rcTitle.left()-titleMargin);
            rcTitle.setRight(rcTitle.right()+titleMargin);

            rc.setBottom(ptCenter.y()-1);
        }
        else if (E_GroupTitlePos::GTP_Left == m_eTitlePos)
        {
            rcTitle.setTop(rcTitle.top()-titleMargin);
            rcTitle.setBottom(rcTitle.bottom()+titleMargin);

            rc.setLeft(ptCenter.x()+1);
        }
        else if (E_GroupTitlePos::GTP_Right == m_eTitlePos)
        {
            rcTitle.setTop(rcTitle.top()-titleMargin);
            rcTitle.setBottom(rcTitle.bottom()+titleMargin);

            rc.setRight(ptCenter.x()-1);
        }
        else
        {
            rcTitle.setLeft(rcTitle.left()-titleMargin);
            rcTitle.setRight(rcTitle.right()+titleMargin);

            rc.setTop(ptCenter.y()+1);
        }

        auto cr = g_crFore;
        cr.setAlpha(CPainter::oppTextAlpha(100));
        painter.drawRectEx(rc, cr, 1, Qt::SolidLine, __szRound);

        //painter.fillRect(rcTitle, g_crBkg); //对话框上显示有背景图水印，覆盖效果不好
        painter.drawTextEx(rcTitle, nAlign, title());
    }
};
