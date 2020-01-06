
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

class CGroupFrame : public CWidget<QGroupBox>
{
    Q_OBJECT
public:
    CGroupFrame(QWidget *parent)
        : CWidget(parent, QPainter::Antialiasing | QPainter::TextAntialiasing)
    {
    }

private:
    E_GroupTitlePos m_eTitlePos = E_GroupTitlePos::GTP_Top;

public:
    void setTitle(const QString& qsTitle, E_GroupTitlePos ePos = E_GroupTitlePos::GTP_Top)
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

        QRect rcText;
        painter.drawTextEx(rc, nAlign, title(), &rcText);

        int margin = __size(18);

        cauto ptCenter = rcText.center();
        if (E_GroupTitlePos::GTP_Bottom == m_eTitlePos)
        {
            rcText.setLeft(rcText.left()-margin);
            rcText.setRight(rcText.right()+margin);

            rc.setBottom(ptCenter.y()-1);
        }
        else if (E_GroupTitlePos::GTP_Left == m_eTitlePos)
        {
            rcText.setTop(rcText.top()-margin);
            rcText.setBottom(rcText.bottom()+margin);

            rc.setLeft(ptCenter.x()+1);
        }
        else if (E_GroupTitlePos::GTP_Right == m_eTitlePos)
        {
            rcText.setTop(rcText.top()-margin);
            rcText.setBottom(rcText.bottom()+margin);

            rc.setRight(ptCenter.x()-1);
        }
        else
        {
            rcText.setLeft(rcText.left()-margin);
            rcText.setRight(rcText.right()+margin);

            rc.setTop(ptCenter.y()+1);
        }

        auto cr = g_crText;
        cr.setAlpha(CPainter::oppTextAlpha(85));
        painter.drawRectEx(rc, 1, cr, Qt::SolidLine, __szRound);

        painter.fillRect(rcText, g_crTheme);
        painter.drawTextEx(rcText, nAlign, title());
    }
};
