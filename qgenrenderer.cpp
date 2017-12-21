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
#include "qgenrenderer.h"

QGenRenderer::QGenRenderer(const QString &family, int size)
{
    fontFamily = family;
    fontSize = size;
    glyphs = new MathGlyphs(QFont(family, size, QFont::Normal, false));
    largeGlyphs = new MathGlyphs(QFont(family, 2 * size, QFont::Normal, false));
}

QString QGenRenderer::paddedText(const QString &text, bool padLeft, bool padRight)
{
    QChar space = MathGlyphs::mediumMathSpace();
    QString padded(text);
    if (padLeft)
        padded.prepend(space);
    if (padRight)
        padded.append(space);
    return padded;
}

QString QGenRenderer::quotedText(const QString &text)
{
    QString quoted(text);
    quoted.prepend(MathGlyphs::leftDoubleQuotationMarkSymbol());
    quoted.append(MathGlyphs::rightDoubleQuotationMarkSymbol());
    return quoted;
}

QString QGenRenderer::numberToSuperscriptText(int integer, bool parens)
{
    bool neg = integer < 0;
    QString digits = QString::number(neg ? -integer : integer);
    QString raised = MathGlyphs::digitsToSuperscript(digits);
    if (neg)
        raised.prepend(MathGlyphs::superscriptMinusSignSymbol());
    if (parens)
    {
        raised.prepend(MathGlyphs::superscriptLeftParenthesisSymbol());
        raised.append(MathGlyphs::superscriptRightParenthesisSymbol());
    }
    return raised;
}

QString QGenRenderer::numberToSubscriptText(int integer, bool parens)
{
    bool neg = integer < 0;
    QString digits = QString::number(neg ? -integer : integer);
    QString raised = MathGlyphs::digitsToSubscript(digits);
    if (neg)
        raised.prepend(MathGlyphs::subscriptMinusSignSymbol());
    if (parens)
    {
        raised.prepend(MathGlyphs::subscriptLeftParenthesisSymbol());
        raised.append(MathGlyphs::subscriptRightParenthesisSymbol());
    }
    return raised;
}

QGenDisplay QGenRenderer::render(const QGen &g, BracketType leftBracket, BracketType rightBracket)
{
    QGenDisplay device;
    if (g.isString())
        renderText(&device, quotedText(g.stringValue()));
    else if (g.isConstant())
        renderNumber(&device, g);
    else if (g.isIdentifier())
        renderIdentifier(&device, g);
    else if (g.isFunction())
        renderFunction(&device, g);
    else if (g.isModular())
        renderModular(&device, g);
    else if (g.isMap())
        renderMap(&device, g);
    else if (g.isVector())
        renderVector(&device, g.toVector());
    else if (g.isSymbolic())
        renderSymbolic(&device, g);
    else
        renderText(&device, g.toString());
    return device;
}

void QGenRenderer::renderDisplayed(QPaintDevice *device, const QPicture &picture, QPointF where)
{
    QPainter painter(device);
    painter.drawPicture(where, picture);
}

void QGenRenderer::renderText(QPaintDevice *device, const QString &text, QPointF where)
{
    QPainter painter(device);
    painter.drawText(where, text);
}

void QGenRenderer::renderNumber(QPaintDevice *device, const QGen &g, QPointF where)
{
    if (g.isFraction())
        renderFraction(device, g.fractionNumerator(), g.fractionDenominator(), where);
    else
    {
        QString text = g.absoluteValue().toString();
        int index;
        if ((index = text.indexOf("e")) >= 0)
        {
            QString mantissa = text.left(index);
            QString exponent = text.mid(index + 1);
            bool hasNegativeExponent = exponent.at(0) == '-';
            if (hasNegativeExponent)
                exponent = exponent.mid(1);
            text = mantissa;
            text.append(paddedText(MathGlyphs::multiplicationSignSymbol()));
            QString exponentDigits = glyphs->digitsToSuperscript(exponent);
            QString sign = hasNegativeExponent ? QString(MathGlyphs::superscriptMinusSignSymbol()) : "";
            text.append("10" + sign + exponentDigits);
        }
        if (g.isNegative())
            text.prepend(MathGlyphs::minusSignSymbol());
        renderText(device, text);
    }
}

void QGenRenderer::renderIdentifier(QPaintDevice *device, const QGen &g, QPointF where)
{
    giac::identificateur ident(*g._IDNTptr);
    QString symbol;
    if (ident == giac::_IDNT_infinity())
        symbol = QString(MathGlyphs::infinitySymbol());
    else if (ident == giac::_IDNT_undef())
        symbol = "undefined";
    else
    {
        QString text = g.toString();
        if (text == "euler_gamma")
            symbol = QString(MathGlyphs::smallGreekGammaSymbol());
        else
        {
            bool italic = text != "pi";
            bool bold = false;
            if (text.startsWith(' '))
            {
                text = text.mid(1);
                bold = true;
            }
            QChar greekLetter;
            if (glyphs->isGreekLetter(text, greekLetter))
                symbol = MathGlyphs::letterToMath(greekLetter, MathGlyphs::Greek, bold, italic);
            else
            {
                QString::iterator it;
                for (it = text.begin(); it != text.end(); ++it)
                {
                    if (it->isLetter())
                        symbol.append(MathGlyphs::letterToMath(*it, MathGlyphs::Serif, bold, italic));
                    else
                        symbol.append(*it);
                }
            }
        }
    }
    renderText(device, symbol, where);
}

void QGenRenderer::renderFunction(QPaintDevice *device, const QGen &g, int exponent, QPointF where)
{
    QPointF penPoint(where);
    QGen args = g.getFeuille();
    bool noParens = g.isElementaryFunction() &&
            (args.isConstant() || args.isIdentifier() || args.isAbsoluteValue() || args.isRational());
    QString symbol = g.sommetName();
    QString digits("");
    if (symbol.length() == 1)
        symbol = QString(MathGlyphs::letterToMath(*symbol.begin(), MathGlyphs::Serif, false, true));
    if (exponent > 0)
        digits = MathGlyphs::digitsToSuperscript(QString::number(exponent));
    symbol += digits;
    symbol.append(MathGlyphs::functionApplicationSpace());
    renderText(device, symbol, penPoint);
    penPoint.setX(penPoint.x() + glyphs->width(symbol));
    QPicture picture = render(args, noParens ? None : LeftParenthesis, noParens ? None : RightParenthesis);
    renderDisplayed(device, picture, penPoint);
}

void QGenRenderer::renderModular(QPaintDevice *device, const QGen &g, QPointF where)
{
    QGen value, modulus;
    g.getModularComponents(value, modulus);
    QGenDisplay valueDisplay = render(value);
    QGenDisplay modulusDisplay = render(modulus);
    QPointF penPoint(0, 0);
    renderDisplayed(device, valueDisplay, penPoint);
    penPoint.setX(penPoint.x() + valueDisplay.advance());
    renderText(device, " (mod ", penPoint);
    penPoint.setX(penPoint.x() + glyphs->width(" (mod "));
    renderDisplayed(device, modulusDisplay, penPoint);
    penPoint.setX(penPoint.x() + modulusDisplay.advance());
    renderText(device, ")", penPoint);
}

void QGenRenderer::renderUnaryOperation(QPaintDevice *device, const QGen &g, QPointF where)
{

}

void QGenRenderer::renderBinaryOperation(QPaintDevice *device, const QGen &g, QPointF where)
{

}

void QGenRenderer::renderAssociativeOperation(QPaintDevice *device, const QGen &g, QPointF where)
{

}

void QGenRenderer::renderFraction(QPaintDevice *device, const QGen &num, const QGen &den, QPointF where)
{

}

void QGenRenderer::renderPower(QPaintDevice *device, const QGen &base, const QGen &exponent, QPointF where)
{

}

void QGenRenderer::renderRoot(QPaintDevice *device, const QGen &argument, int degree, QPointF where)
{

}

void QGenRenderer::renderMap(QPaintDevice *device, const QGen &g, QPointF where)
{

}

void QGenRenderer::renderVector(QPaintDevice *device, const QGen::Vector &v, QPointF where)
{

}

void QGenRenderer::renderSymbolic(QPaintDevice *device, const QGen &g, QPointF where)
{

}
