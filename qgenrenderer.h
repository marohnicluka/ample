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
#include "mathglyphs.h"
#include "qgen.h"

class QGenDisplay : public QPicture
{
public:
    int leftBearing() { return -boundingRect().x(); }
    int advance() { return boundingRect().width() - leftBearing(); }
    int ascent() { return -boundingRect().y(); }
    int descent() { return boundingRect().height() - ascent() - 1; }
    int totalWidth() { return boundingRect().width(); }
    int totalHeight() { return boundingRect().height(); }
};

class QGenRenderer
{
private:
    QString fontFamily;
    int fontSize;

    MathGlyphs *glyphs;
    MathGlyphs *largeGlyphs;

    static QString paddedText(const QString &text, bool padLeft = true, bool padRight = true);
    static QString quotedText(const QString &text);
    static QString numberToSuperscriptText(int integer, bool parens = false);
    static QString numberToSubscriptText(int integer, bool parens = false);

    void renderDisplayed(QPaintDevice *device, const QPicture &picture, QPointF where = QPointF(0, 0));
    void renderText(QPaintDevice *device, const QString &text, QPointF where = QPointF(0, 0));
    void renderNumber(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderIdentifier(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderFunction(QPaintDevice *device, const QGen &g, int exponent = 0, QPointF where = QPointF(0, 0));
    void renderVector(QPaintDevice *device, const QGen::Vector &v, QPointF where = QPointF(0, 0));
    void renderModular(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderMap(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderSymbolic(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderUnaryOperation(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderBinaryOperation(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderAssociativeOperation(QPaintDevice *device, const QGen &g, QPointF where = QPointF(0, 0));
    void renderFraction(QPaintDevice *device, const QGen &num, const QGen &den, QPointF where = QPointF(0, 0));
    void renderPower(QPaintDevice *device, const QGen &base, const QGen &exponent, QPointF where = QPointF(0, 0));
    void renderRoot(QPaintDevice *device, const QGen &argument, int degree, QPointF where = QPointF(0, 0));

public:
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

    QGenRenderer(const QString &family, int size);
    ~QGenRenderer() { delete glyphs; delete largeGlyphs; }

    QGenDisplay render(const QGen &g, BracketType leftBracket = None, BracketType rightBracket = None);
};

#endif // QGENRENDERER_H
