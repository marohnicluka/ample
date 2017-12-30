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

#ifndef QGENRENDERER_H
#define QGENRENDERER_H

#include <QPicture>
#include <QStack>
#include <QFont>
#include <QFontMetricsF>
#include <QRectF>
#include "mathglyphs.h"
#include "qgen.h"

class QGenRenderer
{
    class Display : public QPicture
    {
    public:
        Display() : QPicture() { }
        Display(const QPicture &picture) : QPicture(picture) { }
        Display(const Display &display) : QPicture(display) { }
        int leftBearing() const { return -boundingRect().x(); }
        int advance() const { return boundingRect().width() - leftBearing(); }
        int ascent() const { return -boundingRect().y(); }
        int descent() const { return boundingRect().height() - ascent() - 1; }
        int totalWidth() const { return boundingRect().width(); }
        int totalHeight() const { return boundingRect().height(); }
    };

    enum BracketType
    {
        None,
        LeftParenthesis,
        RightParenthesis,
        LeftSquareBracket,
        RightSquareBracket,
        LeftCurlyBracket,
        RightCurlyBracket,
        LeftFloorBracket,
        RightFloorBracket,
        LeftCeilBracket,
        RightCeilBracket,
        LeftAbsoluteValueBracket,
        RightAbsoluteValueBracket,
        LeftDoubleAbsoluteValueBracket,
        RightDoubleAbsoluteValueBracket
    };

    enum MathPadding
    {
        Hair,
        Thin,
        Medium,
        Thick
    };

    QList<int> fontSizes;
    QString fontFamily;
    QStack<int> fontSizeLevelStack;
    bool renderFontItalic;
    bool renderFontBold;

    int fontSizeLevel() const { Q_ASSERT(!fontSizeLevelStack.empty()); return fontSizeLevelStack.top(); }
    int smallerFontSizeLevel() const { return fontSizeLevel() - 1; }
    int largerFontSizeLevel() const { return fontSizeLevel() + 1; }

    void setRenderingItalic(bool yes) { renderFontItalic = yes; }
    void setRenderingBold(bool yes) { renderFontBold = yes; }

    static QString paddedText(const QString &text, MathPadding padding = Medium, bool padLeft = true, bool padRight = true);
    static QString quotedText(const QString &text);
    static QString numberToSuperscriptText(int integer, bool parens = false);
    static QString numberToSubscriptText(int integer, bool parens = false);
    static QString identifierStringToUnicode(const QString &text, bool bold, bool italic);

    Display render(const QGen &g, int sizeLevel, BracketType leftBracketType, BracketType rightBracketType);
    Display renderNormal(const QGen &g, bool parenthesize = false);
    Display renderSmaller(const QGen &g, bool parenthesize = false);
    Display renderLarger(const QGen &g, bool parenthesize = false);

    void renderLine(QPaintDevice *device, QPointF start, QPointF end);
    void renderHLine(QPaintDevice *device, QPointF start, qreal length);
    qreal renderText(QPaintDevice *device, const QString &text, int relativeFontSizeLevel = 0,
                    QPointF where = QPointF(0, 0), QRectF *boundingRect = Q_NULLPTR);
    void renderTextAndAdvance(QPaintDevice *device, const QString &text, QPointF &penPoint);
    void renderDisplayed(QPaintDevice *device, const Display &displayed, QPointF where = QPointF(0, 0));
    void renderDisplayedAndAdvance(QPaintDevice *device, const Display &displayed, QPointF &penPoint);
    void renderNumber(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderIdentifier(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderLeadingUnderscoreIdentifier(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderFunction(QPaintDevice *device, const QGen &g, int exponent = 0, QPointF where = QPointF(0, 0));
    void renderVector(QPaintDevice *device, const QGen::Vector &v, QPointF where = QPointF(0, 0));
    void renderModular(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderMap(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderSymbolic(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderUnaryOperation(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderBinaryOperation(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderAssociativeOperation(QPaintDevice *device, const QGen &g, const QGen::Vector operands,
                                    QPointF where = QPointF(0, 0));
    void renderFraction(QPaintDevice *device, const QGen &numerator, const QGen &denominator,
                        QPointF where = QPointF(0, 0));
    void renderPower(QPaintDevice *device, const QGen &base, const QGen &exponent,
                     QPointF where = QPointF(0, 0), bool circ = false);
    void renderRadical(QPaintDevice *device, const QGen &argument, int degree, QPointF where = QPointF(0, 0));

    void movePenPointX(QPointF &penPoint, qreal offset) { penPoint.setX(penPoint.x() + offset); }
    void movePenPointY(QPointF &penPoint, qreal offset) { penPoint.setY(penPoint.y() + offset); }
    void movePenPointXY(QPointF &penPoint, qreal offsetX, qreal offsetY)
    {
        movePenPointX(penPoint, offsetX);
        movePenPointY(penPoint, offsetY);
    }

    QFont makeFont(int fontSizeLevel);
    qreal textWidth(const QString &text, int relativeFontSizeLevel = 0);
    QRectF textTightBoundingRect(const QString &text, int relativeFontSizeLevel = 0);
    qreal fontHeight(int relativeFontSizeLevel = 0);
    qreal fontAscent(int relativeFontSizeLevel = 0);
    qreal fontDescent(int relativeFontSizeLevel = 0);
    qreal fontMidLine(int relativeFontSizeLevel = 0);
    qreal fontCenter(int relativeFontSizeLevel = 0);
    qreal fontLeading(int relativeFontSizeLevel = 0);

public:
    QGenRenderer(const QString &family, int size);
    QPicture render(const QGen &g);
};

#endif // QGENRENDERER_H
