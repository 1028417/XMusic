
#include "xmusic.h"

#include "painter.h"

#include <QFontDatabase>

#define __defThemeColor QRGB(130, 200, 255)
#define __defTextColor QRGB(245, 255, 225)

QColor g_crBkg(__defThemeColor);
QColor g_crFore(__defTextColor);

int CFont::g_nDefFontWeight = QFont::Weight::Light;
UINT CFont::m_uDefFontSize = 0;
list<pair<int, QString>> CFont::m_lstFontFamily;

void CFont::init(const QFont& font)
{
#if __ios
    /*int nScreenSize = g_screen.nMaxSide * g_screen.nMinSide;
    switch (nScreenSize)
    {
    case 320*568: // iPhoneSE
        break;
    case 375*667: // iPhone6 iPhone6S iPhone7 iPhone8
        break;
    case 414*736: // iPhone6plus iPhone6Splus iPhone7plus iPhone8plus
        break;
    case 375*812: // iPhoneX iPhoneXS ??
        break;
    case 414*896: // iPhoneXR iPhoneXSmax ??
        break;
    case 1024*1366: // iPadPro1_12.9 iPadPro2_12.9 iPadPro3_12.9
        break;
    case 834*1194: // iPadPro1_11
        break;
    case 834*1112: // iPadPro1_10.5
        break;
    case 768*1024: // iPadPro1_9.7 iPadAir2 iPadAir iPad6 iPad5
        break;
    default:        // iPadMini
        break;
    };*/

    m_uDefFontSize = font.pointSize();
    //m_uDefFontSize *= g_screen.nMaxSide/540.0f;

#elif __mac
    m_uDefFontSize = 28;

#elif __windows
    m_uDefFontSize = 22;

    float fDPIRate = getDPIRate();
    g_logger << "DPIRate: " >> fDPIRate;
    m_uDefFontSize *= fDPIRate;

#elif __android
    m_uDefFontSize = 12;
#endif

#if __windows
    m_lstFontFamily.emplace_back(QFont::Weight::Light, "微软雅黑 Light");
    m_lstFontFamily.emplace_back(QFont::Weight::DemiBold, "微软雅黑");
    return;
#endif

    auto qsFontName = font.family();
    list<pair<int, QString>> plFontFile {
        {QFont::Weight::Light, "msyhl-6.23.ttc"}
        , {QFont::Weight::DemiBold, "Microsoft-YaHei-Regular-11.0.ttc"}
    };
    for (auto& pr : plFontFile)
    {
        auto qsFontFile = "/font/" + pr.second;
#if __android
        qsFontFile = "assets:" +  qsFontFile;
#else
        qsFontFile = CApp::applicationDirPath() + qsFontFile;
#endif

        int fontId = QFontDatabase::addApplicationFont(qsFontFile);
        if (-1 != fontId)
        {
            cauto qslst = QFontDatabase::applicationFontFamilies(fontId);
            if (!qslst.empty())
            {
                qsFontName =  qslst.front();
                g_logger << "addFont: " << qsFontFile << ", familyName: " >> qsFontName;
            }
        }
        else
        {
            g_logger << "addFont fail: " >> qsFontFile;
        }

        m_lstFontFamily.emplace_back(pr.first, qsFontName);
    }
}

void CPainter::alphaPixmap(cqpm pmSrc, int alpha, QPixmap& pmDst)
{
    pmDst.fill(Qt::transparent);
    QPainter painter(&pmDst);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawPixmap(0, 0, pmSrc);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.fillRect(pmDst.rect(), QColor(0, 0, 0, alpha));
    painter.end();
}

QColor CPainter::mixColor(cqcr crSrc, cqcr crDst, UINT uAlpha)
{
    uAlpha = uAlpha * crSrc.alpha()/255 * crDst.alpha()/255;

    int r = MIN(crSrc.red(), crDst.red());
    int g = MIN(crSrc.green(), crDst.green());
    int b = MIN(crSrc.blue(), crDst.blue());

    r += (-r + MAX(crSrc.red(), crDst.red()))*uAlpha / 255;
    g += (-g + MAX(crSrc.green(), crDst.green()))*uAlpha / 255;
    b += (-b + MAX(crSrc.blue(), crDst.blue()))*uAlpha / 255;

    return QColor(r,g,b,uAlpha);
}

#define __painterRestoreGuard CPainterRestoreGuard _guard(*this)

void CPainter::drawImg(cqrc rc, QBrush& br, cqrc rcSrc, UINT xround, UINT yround)
{
    QTransform transform;

    auto fWScaleRate = Double_T(rc.width())/rcSrc.width();
    auto fHScaleRate = Double_T(rc.height())/rcSrc.height();
    transform.translate(rc.left()-rcSrc.left()*fWScaleRate, rc.top()-rcSrc.top()*fHScaleRate);
    transform.scale(fWScaleRate, fHScaleRate);
    br.setTransform(transform);

    __painterRestoreGuard;

    this->setBrush(br);
    this->setPen(Qt::transparent);

    _drawRectEx(rc, xround, yround);
}

void CPainter::drawImgEx(cqrc rc, QBrush& br, cqrc rcSrc, UINT xround, UINT yround)
{
    QRect t_rcSrc = rcSrc;
    _genSrcRect(rc, t_rcSrc);
    this->drawImg(rc, br, t_rcSrc, xround, yround);
}

void CPainter::drawImgEx(cqrc rc, cqpm pm, int& dx, int& dy, UINT szAdjust)
{
    QRect rcSrc = pm.rect();
    _genSrcRect(rc, rcSrc);

    if (dx != 0 || dy != 0)
    {
        auto fScaleRate = float(szAdjust+rcSrc.width())/rc.width();
        if (dx != 0)
        {
            auto offset = dx*fScaleRate;
            auto lpos = rcSrc.left()+offset;
            if (lpos < 0)
            {
                dx = -rcSrc.left()/fScaleRate;

                rcSrc.setRight(rcSrc.right()-rcSrc.left());
                rcSrc.setLeft(0);
            }
            else
            {
                auto rpos = rcSrc.right()+offset;
                auto cx = pm.width();
                if (rpos >= cx)
                {
                    dx = (cx-1-rcSrc.right())/fScaleRate;

                    rcSrc.setLeft(cx-rcSrc.width());
                    rcSrc.setRight(cx-1);
                }
                else
                {
                    rcSrc.setLeft(lpos);
                    rcSrc.setRight(rpos);
                }
            }
        }

        if (dy != 0)
        {
            auto offset = dy*fScaleRate;
            auto tpos = rcSrc.top()+offset;
            if (tpos < 0)
            {
                dy = -rcSrc.top()/fScaleRate;

                rcSrc.setBottom(rcSrc.bottom()-rcSrc.top());
                rcSrc.setTop(0);
            }
            else
            {
                auto bpos = rcSrc.bottom()+offset;
                auto cy = pm.height();
                if (bpos >= cy)
                {
                    dy = (cy-1-rcSrc.bottom())/fScaleRate;

                    rcSrc.setTop(cy-rcSrc.height());
                    rcSrc.setBottom(cy-1);
                }
                else
                {
                    rcSrc.setTop(tpos);
                    rcSrc.setBottom(bpos);
                }
            }
        }
    }

    this->drawImg(rc, pm, rcSrc);
}

void CPainter::_drawRectEx(cqrc rc, UINT xround, UINT yround)
{
    if (xround > 0)
    {
        if (0 == yround)
        {
            yround = xround;
        }
        this->drawRoundedRect(rc, xround, yround);
    }
    else
    {
        this->drawRect(rc);
    }
}

void CPainter::drawRectEx(cqrc rc, cqcr cr, Qt::PenStyle style, UINT uWidth, UINT xround, UINT yround)
{
    __painterRestoreGuard;

    QPen pen;
    pen.setColor(cr);
    pen.setWidth(uWidth);
    pen.setStyle(style);
    this->setPen(pen);

    _drawRectEx(rc, xround, yround);
}

void CPainter::fillRectEx(cqrc rc, cqbr br, UINT xround, UINT yround)
{
    if (xround > 0)
    {
        if (0 == yround)
        {
            yround = xround;
        }

        __painterRestoreGuard;

        this->setPen(Qt::transparent);
        this->setBrush(br);

        this->drawRoundedRect(rc, xround, yround);
    }
    else
    {
        this->fillRect(rc, br);
    }
}

void CPainter::fillRectEx(cqrc rc, cqcr crBegin
                , cqcr crEnd, UINT xround, UINT yround)
{
    QLinearGradient gradient(rc.topLeft(), rc.topRight());
    gradient.setColorAt(0, crBegin);
    gradient.setColorAt(1, crEnd);
    QBrush br(gradient);

    fillRectEx(rc, br, xround, yround);
}

static QRect g_rcDrawTextRet;

cqrc CPainter::drawTextEx(cqrc rc, int flags, cqstr qsText, cqcr crText
                          , UINT uShadowWidth, UINT uShadowAlpha, UINT uTextAlpha)
{
    __painterRestoreGuard;

    if (uShadowWidth > 0 && uShadowAlpha > 0)
    {
        int r = crText.red();
        int g = crText.green();
        int b = crText.blue();
        QColor crShadow(r<128?r+128:r-128, g<128?g+128:g-128, b<128?b+128:b-128, uShadowAlpha/(uShadowWidth+1));

        for (UINT uIdx=0; uIdx<=uShadowWidth; uIdx++)
        {
            if (uIdx > 0)
            {
                crShadow.setAlpha(uShadowAlpha*(uShadowWidth-uIdx+1)/(uShadowWidth+1));
            }

            this->setPen(crShadow);

            QRect rcShadow(rc.left()+uIdx, rc.top()+uIdx, rc.width(), rc.height());
            QPainter::drawText(rcShadow, flags, qsText);
        }
    }

    QColor t_crText = crText;
    t_crText.setAlpha(uTextAlpha);
    this->setPen(t_crText);
    QPainter::drawText(rc, flags, qsText, &g_rcDrawTextRet);

    return g_rcDrawTextRet;
}
