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
#include "mathdisplaywidget.h"
#include "mathglyphs.h"

MathDisplayWidget::MathDisplayWidget(QWidget *parent) : QWidget(parent)
{

}

void drawExtensionFill(QPainter &painter, const QChar &extension, qreal x, qreal yLower, qreal yUpper)
{
    QFont font("FreeSerif", 12);
    QFontMetricsF fontMetrics(font);
    qreal unit = fontMetrics.ascent() + fontMetrics.leading();
    qreal y = yLower;
    while (y - unit >= yUpper)
    {
        painter.drawText(QPointF(x, y), extension);
        y -= unit;
    }
    qreal range = y + fontMetrics.descent() - yUpper + 1;
    qreal f = range / unit;
    y += (1 - f) * fontMetrics.descent();
    painter.translate(x, y);
    painter.scale(1.0, f);
    painter.drawText(QPointF(0.0, 0.0), extension);
}

void drawLeftParenthesis(QPainter &painter, int x, int yLower, int yUpper)
{
    //painter.drawLine(QPoint(x, yLower), QPoint(x, yUpper));
    painter.save();
    QFont font("FreeSerif", 12);
    QFontMetricsF fontMetrics(font);
    painter.setFont(font);
    qreal unit = fontMetrics.ascent() + fontMetrics.leading();
    qreal xStart = x - fontMetrics.width(MathGlyphs::leftSquareBracketUpperCorner());
    qreal upperHookYStart = yUpper + fontMetrics.ascent() - fontMetrics.leading();
    painter.drawText(QPointF(xStart, upperHookYStart - fontMetrics.descent()),
                     MathGlyphs::leftSquareBracketUpperCorner());
    painter.drawText(QPointF(xStart, yLower), MathGlyphs::leftSquareBracketLowerCorner());
    drawExtensionFill(painter, MathGlyphs::leftSquareBracketExtension(), xStart, yLower - unit, upperHookYStart);
    painter.restore();
}

void drawSummationSymbol(QPainter &painter, QPointF where)
{
    painter.save();
    QFont font("FreeSerif", 12);
    painter.setFont(font);
    QFontMetrics fontMetrics(font);
    qreal x = where.x(), y = where.y() - fontMetrics.xHeight();
    qreal ascent = -fontMetrics.tightBoundingRect(MathGlyphs::upperLeftCurlyBracketSection()).y();
    QRect rect = fontMetrics.tightBoundingRect(MathGlyphs::lowerLeftCurlyBracketSection());
    qreal descent = rect.height() + rect.y();
    qreal skip = fontMetrics.ascent() + fontMetrics.leading();
    painter.drawText(x, y, MathGlyphs::upperLeftCurlyBracketSection());
    painter.drawText(x, y + skip, MathGlyphs::lowerLeftCurlyBracketSection());
    painter.drawLine(x, y-ascent, x+30, y-ascent);
    painter.drawLine(x,y+skip+descent, x+30,y+skip+descent);
    painter.restore();
}

void MathDisplayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    for (int i = 1; i < 15; ++i)
    {
        drawLeftParenthesis(painter, width() / 2 + i * 20, height() / 2 + 10 + 10*i, height() / 2 - 10 - 10*i);
    }
    QFont font("FreeSerif", 45);
    QFontMetrics fontMetrics(font);
    painter.setFont(font);
    drawSummationSymbol(painter, QPointF(width() / 3, height() / 2));
    painter.drawText(width() / 3 + 25, height() / 2,
                     MathGlyphs::letterToMath('a', MathGlyphs::Serif, false, true));
    //painter.setRenderHint(QPainter::Antialiasing);
    QRect rect = fontMetrics.tightBoundingRect(MathGlyphs::squareRootSymbol());
    painter.translate(width() / 4, height() / 2);
    QPointF pt((rect.x() + rect.width()) * 0.3555 - 1, rect.y());
    painter.drawLine(pt, QPointF(pt.x() + 30, pt.y()));
    painter.drawLine(0.0, 0.0 - (fontMetrics.xHeight() / 2 + fontMetrics.strikeOutPos()) / 2, -20, 0.0 - fontMetrics.xHeight() / 2);
    painter.scale(0.355,1.0);
    painter.drawText(QPoint(0,0), MathGlyphs::squareRootSymbol());
}
