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
        bool m_grouped;
        int m_priority;

    public:
        Display() : QPicture() { m_grouped = false; m_priority = 0; }
        Display(const QPicture &picture) : QPicture(picture) { m_grouped = false; m_priority = 0; }
        Display(const Display &display) : QPicture(display) { m_grouped = false; m_priority = 0; }
        int priority() const { return m_priority; }
        bool isGrouped() const { return m_grouped; }
        void setPriority(int value) { m_priority = value; }
        void setGrouped(bool yes) { m_grouped = yes; }
        int leftBearing() const { return -boundingRect().x(); }
        int advance() const { return boundingRect().width() - leftBearing(); }
        int ascent() const { return -boundingRect().y(); }
        int descent() const { return boundingRect().height() - ascent(); }
        int totalWidth() const { return boundingRect().width(); }
        int totalHeight() const { return boundingRect().height(); }
    };

    enum BracketType
    {
        None, Parenthesis, WhiteParenthesis, SquareBracket, WhiteSquareBracket, CurlyBracket, WhiteCurlyBracket,
        FloorBracket, CeilingBracket, AngleBracket, StraightBracket, DoubleStraightBracket
    };

    enum AccentType { Hat, Check, Tilde, Acute, Grave, Dot, DoubleDot, TripleDot, Bar, Vec };

    enum MathPadding { Hair, Thin, Medium, Thick };

    QList<int> fontSizes;
    QList<QFont> fonts;
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

    void render(Display &dest, const QGen &g, int sizeLevel = 0);
    Display renderNormal(const QGen &g);
    Display renderSmaller(const QGen &g);
    Display renderLarger(const QGen &g);

    void renderHorizontalLine(QPainter &painter, qreal x, qreal y, qreal length, qreal widthFactor = 1.0);
    void renderHorizontalLine(Display &dest, QPointF where, qreal length, qreal widthFactor = 1.0);
    qreal renderText(Display &dest, const QString &text, int relativeFontSizeLevel = 0,
                    QPointF where = QPointF(0, 0), QRectF *boundingRect = Q_NULLPTR);
    void renderTextAndAdvance(Display &dest, const QString &text, QPointF &penPoint);
    void renderBracketExtensionFill(QPainter &painter, const QChar &extension,
                                    qreal x, qreal yLower, qreal yUpper);
    qreal renderSingleBracket(QPainter &painter, qreal x, qreal halfHeight, QChar bracket, bool scaleX = false);
    qreal renderSingleFillBracket(QPainter &painter, qreal x, qreal halfHeight,
                                  QChar upperPart, QChar lowerPart, QChar extension, QChar singleBracket);
    qreal renderCurlyBracket(QPainter &painter, qreal x, qreal halfHeight, bool left);
    void renderDisplay(Display &dest, const Display &source, QPointF where = QPointF(0, 0));
    void renderDisplayWithRadical(Display &dest, const Display &source, QPointF where = QPointF(0, 0));
    void renderDisplayWithAccent(Display &dest, const Display &source, AccentType accentType,
                                 QPointF where = QPointF(0, 0));
    qreal renderDisplayWithBrackets(Display &dest, const Display &source,
                                    BracketType leftBracketType, BracketType rightBracketType,
                                    QPointF where = QPointF(0, 0));
    qreal renderDisplayWithParentheses(Display &dest, const Display &source, QPointF where = QPointF(0, 0));
    qreal renderDisplayWithPriority(Display &dest, const Display &source, int priority, QPointF where = QPointF(0, 0));
    qreal renderDisplayWithSquareBrackets(Display &dest, const Display &source, QPointF where = QPointF(0, 0));
    qreal renderDisplayWithCurlyBrackets(Display &dest, const Display &source, QPointF where = QPointF(0, 0));
    void renderDisplayAndAdvance(Display &dest, const Display &source, QPointF &penPoint);
    void renderNumber(Display &dest, const QGen &g, QPointF where = QPointF(0, 0));
    void renderIdentifier(Display &dest, const QGen &g, QPointF where = QPointF(0, 0));
    void renderLeadingUnderscoreIdentifier(Display &dest, const QGen &g, QPointF where = QPointF(0, 0));
    void renderFunction(Display &dest, const QGen &g, QPointF where = QPointF(0, 0));
    void renderVector(Display &dest, const QGen::Vector &v, QPointF where = QPointF(0, 0));
    void renderModular(Display &dest, const QGen &g, QPointF where = QPointF(0, 0));
    void renderMap(Display &dest, const QGen &g, QPointF where = QPointF(0, 0));
    void renderSymbolic(Display &dest, const QGen &g, QPointF where = QPointF(0, 0));
    void renderUnary(Display &dest, const QGen &g, QPointF where = QPointF(0, 0));
    void renderBinary(Display &dest, const QGen &g, QPointF where = QPointF(0, 0));
    void renderAssociative(Display &dest, const QGen &g, const QGen::Vector operands,
                           QPointF where = QPointF(0, 0));
    void renderFraction(Display &dest, const QGen &numerator, const QGen &denominator,
                        QPointF where = QPointF(0, 0));
    void renderPower(Display &dest, const QGen &base, const QGen &exponent,
                     QPointF where = QPointF(0, 0), bool circ = false);

    void movePenPointX(QPointF &penPoint, qreal offset) { penPoint.setX(penPoint.x() + offset); }
    void movePenPointY(QPointF &penPoint, qreal offset) { penPoint.setY(penPoint.y() + offset); }
    void movePenPointXY(QPointF &penPoint, qreal offsetX, qreal offsetY)
    {
        movePenPointX(penPoint, offsetX);
        movePenPointY(penPoint, offsetY);
    }

    const QFont &getFont(int fontSizeLevel);
    qreal textWidth(const QString &text, int relativeFontSizeLevel = 0);
    QRectF textTightBoundingRect(const QString &text, int relativeFontSizeLevel = 0);
    qreal fontHeight(int relativeFontSizeLevel = 0);
    qreal fontAscent(int relativeFontSizeLevel = 0);
    qreal fontDescent(int relativeFontSizeLevel = 0);
    qreal fontXHeight(int relativeFontSizeLevel = 0);
    qreal fontMidLine(int relativeFontSizeLevel = 0);
    qreal fontLeading(int relativeFontSizeLevel = 0);
    qreal lineWidth(int relativeFontSizeLevel = 0);
    qreal linePadding(int relativeSizeLevel = 0);

public:
    enum Alignment {
        AlignLeft, AlignRight, AlignTop, AlignBottom,
        AlignBaseline, AlignHCenter, AlignVCenter, AlignCenter
    };

    QGenRenderer(const QString &family, int fontSize);

    QPicture render(const QGen &g, int alignment = AlignLeft | AlignBaseline) const;
};

#endif // QGENRENDERER_H
