/*
 * This file is part of Ample.
 *
 * Ample is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ample is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ample.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <qmath.h>
#include "mathdisplaywidget.h"
#include "mathglyphs.h"

#define FONTSIZE 12

MathDisplayWidget::MathDisplayWidget(QWidget *parent) : QWidget(parent)
{

}

void renderBracketExtensionFill(QPainter &painter, const QChar &extension, qreal x, qreal yLower, qreal yUpper)
{
    painter.save();
    QFont font("FreeSerif", FONTSIZE);
    QFontMetricsF fontMetrics(font);
    QRectF rect = fontMetrics.boundingRect(extension);
    qreal offset = rect.y() + rect.height(), y = yLower, skip = rect.height() - 2;
    int n = qFloor(qAbs(yUpper - yLower) / skip);
    for (int i = 0; i < n; ++i)
    {
        painter.drawText(QPointF(x, y - offset), extension);
        y -= skip;
    }
    qreal f = (y - yUpper + 1)  / skip;
    if (f > 0)
    {
        painter.scale(1.0, f);
        painter.drawText(QPointF(x, y / f - offset), extension);
    }
    painter.restore();
}

void drawLeftParenthesis(QPainter &painter, int x, int baseLine, int halfHeight)
{
    painter.save();
    QFont font("FreeSerif", FONTSIZE);
    QFontMetricsF fontMetrics(font);
    painter.setFont(font);
    qreal midLineOffset= (fontMetrics.xHeight() / 2 + fontMetrics.strikeOutPos()) / 2;
    QChar bracket = '(';
    QChar upperHook = MathGlyphs::leftParenthesisUpperHook();
    QChar lowerHook = MathGlyphs::leftParenthesisLowerHook();
    QChar extension = MathGlyphs::leftParenthesisExtension();
    QRectF rect = fontMetrics.tightBoundingRect(upperHook);
    qreal upperHookHeight = rect.height() - 2, upperHookDescent = rect.y() + rect.height();
    if (halfHeight < upperHookHeight)
    {
        rect = fontMetrics.tightBoundingRect(bracket);
        qreal f = qMax(1.0, halfHeight / -(1 + midLineOffset + rect.y()));
        painter.scale(1.0, f);
        painter.drawText(QPointF(x, baseLine / f + midLineOffset * (f - 1) / f), bracket);
    }
    else
    {
        qreal yLower = baseLine + 0.5 - midLineOffset + halfHeight;
        qreal yUpper = baseLine + 0.5 - midLineOffset - halfHeight;
        rect = fontMetrics.tightBoundingRect(lowerHook);
        qreal lowerHookHeight = rect.height() - 2, lowerHookDescent = rect.y() + rect.height();
        painter.drawText(QPointF(x, yUpper + upperHookHeight - upperHookDescent), upperHook);
        painter.drawText(QPointF(x, yLower - lowerHookDescent), lowerHook);
        renderBracketExtensionFill(painter, extension, x, yLower - lowerHookHeight, yUpper + upperHookHeight);
    }
    painter.restore();
}

void drawSummationSymbol(QPainter &painter, QPointF where)
{
    painter.save();
    QFont font("FreeSerif", FONTSIZE);
    painter.setFont(font);
    QFontMetrics fontMetrics(font);
    qreal x = where.x(), y = where.y() - fontMetrics.xHeight();
    QRect rect = fontMetrics.tightBoundingRect(MathGlyphs::lowerLeftCurlyBracketSection());
    qreal ascent = -rect.y(), descent = rect.height() + rect.y();
    qreal skip = fontMetrics.ascent() + fontMetrics.leading();
    painter.drawText(x, y, MathGlyphs::upperLeftCurlyBracketSection());
    painter.drawText(x, y + skip, MathGlyphs::lowerLeftCurlyBracketSection());
    painter.drawLine(x, y-ascent, x+30, y-ascent);
    painter.drawLine(x,y+skip+descent, x+30,y+skip+descent);
    painter.restore();
}

void drawCurlyBracket(QPainter &painter, qreal x, qreal baseLine, qreal halfHeight)
{
    painter.save();
    QFont font("FreeSerif", FONTSIZE);
    QFontMetricsF fontMetrics(font);
    painter.setFont(font);
    qreal midLineOffset= (fontMetrics.xHeight() / 2 + fontMetrics.strikeOutPos()) / 2;
    QRectF rect = fontMetrics.tightBoundingRect(MathGlyphs::leftCurlyBracketUpperHook());
    qreal upperHookHeight = rect.height() - 2, upperHookDescent = rect.y() + rect.height();
    rect = fontMetrics.tightBoundingRect(MathGlyphs::leftCurlyBracketLowerHook());
    qreal lowerHookHeight = rect.height() - 2, lowerHookDescent = rect.y() + rect.height();
    rect = fontMetrics.tightBoundingRect(MathGlyphs::leftCurlyBracketMiddlePiece());
    qreal middlePieceHeight = rect.height() - 2, middlePieceDescent = rect.y() + rect.height();
    rect = fontMetrics.tightBoundingRect(MathGlyphs::upperLeftCurlyBracketSection());
    qreal halfBraceHeight = rect.height() - 2, halfBraceDescent = rect.y() + rect.height();
    qreal offset = fontMetrics.xHeight() - midLineOffset;
    if (halfHeight >= fontMetrics.xHeight() + middlePieceDescent + lowerHookHeight)
    {
        qreal upperHookYStart = baseLine + 0.5 - halfHeight + upperHookHeight - midLineOffset;
        qreal lowerHookYStart = baseLine + 0.5 + halfHeight - lowerHookHeight - midLineOffset;
        qreal y = baseLine + 2 * offset;
        painter.drawText(QPointF(x, y - middlePieceDescent), MathGlyphs::leftCurlyBracketMiddlePiece());
        painter.drawText(QPointF(x, upperHookYStart - upperHookDescent - 0.5), MathGlyphs::leftCurlyBracketUpperHook());
        painter.drawText(QPointF(x, lowerHookYStart + lowerHookHeight - lowerHookDescent),
                         MathGlyphs::leftCurlyBracketLowerHook());
        renderBracketExtensionFill(painter, MathGlyphs::leftCurlyBracketExtension(), x, y - middlePieceHeight + 1, upperHookYStart);
        renderBracketExtensionFill(painter, MathGlyphs::leftCurlyBracketExtension(), x, lowerHookYStart + 1, y);
    }
    else if (halfHeight >= halfBraceHeight)
    {
        qreal f = halfHeight / halfBraceHeight;
        painter.translate(x, baseLine + 0.5 - midLineOffset);
        painter.scale(1.0, f);
        painter.drawText(QPointF(0.0, -halfBraceDescent - 0.5), MathGlyphs::upperLeftCurlyBracketSection());
        painter.drawText(QPointF(0.0, halfBraceHeight - halfBraceDescent),
                         MathGlyphs::lowerLeftCurlyBracketSection());
    }
    else
    {
        rect = fontMetrics.tightBoundingRect("{");
        qreal f = qMax(1.0, halfHeight / -(1 + rect.y() + midLineOffset));
        painter.scale(1.0, f);
        painter.drawText(QPointF(x, baseLine / f + midLineOffset * (f - 1) / f), "{");
    }
    painter.restore();
}

void drawHLine(QPainter &painter, qreal x, qreal y, qreal length, qreal widthFactor = 1.0)
{
    QFont font("FreeSerif", FONTSIZE);
    QFontMetrics fontMetrics(font);
    QRectF rect = fontMetrics.tightBoundingRect(MathGlyphs::horizontalLineExtension());
    qreal xOffset = rect.x() + 0.5, yOffset = rect.y() + rect.height() / 2, baseWidth = rect.width() - 1;
    int n = qRound(length / baseWidth);
    for (int i = 0; i < n; ++i)
        painter.drawText(QPointF(x + i * baseWidth - xOffset, y - yOffset), MathGlyphs::horizontalLineExtension());
    qreal f = length / baseWidth - n;
    if (f > 0)
    {
        painter.save();
        painter.translate(x + n * baseWidth, y);
        painter.scale(f, 1.0);
        painter.drawText(QPointF(-xOffset, -yOffset), MathGlyphs::horizontalLineExtension());
        painter.restore();
    }
}

void drawRadical(QPainter &painter, qreal x, qreal y, qreal h)
{
    QFont font("FreeSerif", FONTSIZE);
    QFontMetrics fontMetrics(font);
    QRectF rect = fontMetrics.tightBoundingRect(MathGlyphs::squareRoot());
    qreal radicalWidth = rect.width();
    qreal radicalHeight = rect.height();
    qreal radicalDescent = rect.y() + rect.height();
    qreal radicalLeftBearing = -rect.x();
    rect = fontMetrics.tightBoundingRect(MathGlyphs::horizontalLineExtension());
    qreal fY = qMax(1.0, (h + rect.height() / 3) / radicalHeight), fX = qPow(fY, 0.2);
    painter.save();
    painter.translate(x, y);
    drawHLine(painter, radicalWidth * fX - rect.height() / 3, -qMax(h, radicalHeight), 50 + fX);
    painter.scale(fX, fY);
    painter.drawText(QPointF(radicalLeftBearing, -radicalDescent), MathGlyphs::squareRoot());
    painter.restore();
}

void MathDisplayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QFont font("FreeSerif", FONTSIZE);
    QFontMetrics fontMetrics(font);
    qreal centerLine = (fontMetrics.xHeight() / 2 + fontMetrics.strikeOutPos()) / 2;
    painter.setFont(font);
    drawRadical(painter, width() / 2, height() / 2,
                2 * fontMetrics.tightBoundingRect(MathGlyphs::squareRoot()).height());
    //painter.drawLine(QPointF(0, height() / 2 - centerLine), QPointF(width() / 2, height() / 2 - centerLine));
    drawSummationSymbol(painter, QPointF(width() / 3, height() / 2));
    painter.drawText(width() / 3 + 25, height() / 2, "⎷");
    //painter.setRenderHint(QPainter::Antialiasing);
    /*
    QRect rect = fontMetrics.tightBoundingRect(MathGlyphs::squareRootSymbol());
    painter.translate(width() / 4, height() / 2);
    QPointF pt((rect.x() + rect.width()) * 0.3555 - 1, rect.y());
    painter.drawLine(pt, QPointF(pt.x() + 30, pt.y()));
    qreal centerLine = (fontMetrics.xHeight() / 2 + fontMetrics.strikeOutPos()) / 2;
    painter.drawLine(0.0, 0.0 - centerLine, -20, 0.0 - fontMetrics.xHeight() / 2);
    //painter.scale(0.355,1.0);
    qreal yOffset = fontMetrics.xHeight() - centerLine;
    painter.drawText(QPoint(0,yOffset), MathGlyphs::leftCurlyBracketMiddlePiece());
    */
}
