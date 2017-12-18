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
#include <sstream>
#include "equation.h"

Equation::Equation(const giac::gen &e, const giac::context *ctx, const QString &family, int size)
    : QPicture()
    , contextptr(ctx)
    , expression(e)
    , fontFamily(family)
    , fontSize(size)
    , glyphs(QFont(family, size))
    , bigGlyphs(QFont(family, size * 2))
{
}

void Equation::setBracketType(int leftType, int rightType)
{
    leftBracketType = leftType;
    rightBracketType = rightType >= 0 ? rightType : leftType;
}

QString Equation::paddedString(QString str, bool padLeft, bool padRight)
{
    QChar space = Glyphs::mediumMathematicalSpace();
    QString padded(str);
    if (padLeft)
        padded.prepend(space);
    if (padRight)
        padded.append(space);
}

QString Equation::toString(const giac::gen &g)
{
    std::stringstream ss;
    ss << g;
    return QString(ss.str().data());
}

QString Equation::numberToSuperscript(int integer, bool parens)
{
    bool neg = integer < 0;
    QString digits = QString::number(neg ? -integer : integer);
    QString raised = glyphs.digitsToSuperscript(digits);
    if (neg)
        raised.prepend(Glyphs::superscriptMinusSignSymbol());
    if (parens)
    {
        raised.prepend(Glyphs::superscriptLeftParenthesisSymbol());
        raised.append(Glyphs::superscriptRightParenthesisSymbol());
    }
    return raised;
}

QString Equation::numberToSubscript(int integer, bool parens)
{
    bool neg = integer < 0;
    QString digits = QString::number(neg ? -integer : integer);
    QString raised = glyphs.digitsToSubscript(digits);
    if (neg)
        raised.prepend(Glyphs::subscriptMinusSignSymbol());
    if (parens)
    {
        raised.prepend(Glyphs::subscriptLeftParenthesisSymbol());
        raised.append(Glyphs::subscriptRightParenthesisSymbol());
    }
    return raised;
}

bool Equation::isNegative(const giac::gen &g)
{
    return  (isSymbolic(g) && isSommet(g, giac::at_neg)) ||
            (type(g) == giac::_INT_ && g.val<0) ||
            (type(g) == giac::_DOUBLE_ && g.DOUBLE_val()<0) ||
            (isFraction(g) && !giac::is_positive(fractionNumerator(g),contextptr));
}

bool Equation::isElementaryFunction(const giac::gen &g) {
    Q_ASSERT(isSymbolic(g));
    return  isSommet(g, giac::at_ln) || isSommet(g, giac::at_exp) || isSommet(g, giac::at_sin) ||
            isSommet(g, giac::at_cos) || isSommet(g, giac::at_tan) || isSommet(g, giac::at_cot) ||
            isSommet(g, giac::at_asin) || isSommet(g, giac::at_acos) || isSommet(g, giac::at_atan) ||
            isSommet(g, giac::at_acot) || isSommet(g, giac::at_sinh) || isSommet(g, giac::at_cosh) ||
            isSommet(g, giac::at_tanh) || isSommet(g, giac::at_asinh) ||
            isSommet(g, giac::at_acosh) || isSommet(g, giac::at_atanh);
}

const giac::vecteur Equation::assocOperands(const giac::gen &g)
{
    Q_ASSERT(g.type == giac::_SYMB);
    giac::vecteur operands(toVector(getFeuille(g)));
    for (int i = 0 ; i < int(operands.size()); ++i) {
        if (operands[i].is_symb_of_sommet(getSommet(g))) {
            giac::vecteur subOperands(assocOperands(operands[i]));
            operands.erase(operands.begin()+i);
            giac::const_iterateur it;
            for (it = subOperands.begin(); it != subOperands.end(); ++it) {
                operands.insert(operands.begin()+i, *it);
                ++i;
            }
        }
    }
    return operands;
}

void Equation::render()
{
    switch (type())
    {
    case giac::_STRNG:
        renderString();
        break;
    case giac::_INT_:
    case giac::_DOUBLE_:
    case giac::_FRAC:
        renderNumber();
        break;
    case giac::_IDNT:
        renderIdentifier();
        break;
    case giac::_FUNC:
        renderFunction();
        break;
    case giac::_MOD:
        renderModulo();
        break;
    case giac::_MAP:
        renderMap();
        break;
    case giac::_VECT:
        renderVector();
        break;
    case giac::_SYMB:
        renderSymbolic();
        break;
    case giac::_ZINT:
    case giac::_REAL:
    case giac::_POLY:
    case giac::_SPOL1:
    case giac::_EXT:
    case giac::_ROOT:
    case giac::_USER:
    case giac::_EQW:
    case giac::_GROB:
    case giac::_POINTER_:
    case giac::_FLOAT_:
    default:
        renderText(toString());
    }
}

QPointF Equation::renderText(QString text, QPointF where)
{
    QPainter painter;
    painter.begin(this);
    painter.drawText(QPointF(where.x()-glyphs.leftBearing(*text.begin()),where.y()), text);
    return QPointF(where.x() + glyphs.boundingRect(text).width(), where.y());
}

void Equation::renderString()
{
    Q_ASSERT(isString());
    QString text(*expression._STRNGptr->data());
    text.prepend(Glyphs::leftDoubleQuotationMarkSymbol());
    text.append(Glyphs::rightDoubleQuotationMarkSymbol());
    renderText(text);
}

void Equation::renderNumber()
{
    if (isFraction())
        renderFraction(fractionNumerator(), fractionDenominator());
    else
    {
        QString text = toString(absoluteValue());
        int index;
        if ((index = text.indexOf("e")) >= 0)
        {
            QString mantissa = text.left(index);
            QString exponent = text.mid(index + 1);
            bool negativeExponent = exponent.at(0) == '-';
            if (negativeExponent)
                exponent = exponent.mid(1);
            text = mantissa;
            text.append(paddedString(Glyphs::multiplicationSignSymbol()));
            QString exponentDigits = glyphs.digitsToSuperscript(exponent);
            QString sign = negativeExponent ? QString(Glyphs::superscriptMinusSignSymbol()) : "";
            text.append("10" + sign + exponentDigits);
        }
        if (isNegative())
            text.prepend(Glyphs::minusSignSymbol());
        renderText(text);
    }
}

void Equation::renderIdentifier()
{
    Q_ASSERT(isIdentifier());
    giac::identificateur ident(*expression._IDNTptr);
    QString symbol;
    if (ident == giac::_IDNT_infinity())
        symbol = QString(Glyphs::infinitySymbol());
    else if (ident == giac::_IDNT_undef())
        symbol = "undefined";
    else
    {
        QString text = toString();
        if (text == "euler_gamma")
            symbol = QString(Glyphs::smallGreekGammaSymbol());
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
            if (glyphs.isGreekLetter(text, greekLetter))
                symbol = Glyphs::letterToMath(greekLetter, Glyphs::Greek, bold, italic);
            else
            {
                QString::iterator it;
                for (it = text.begin(); it != text.end(); ++it)
                {
                    if (it->isLetter())
                        symbol.append(Glyphs::letterToMath(*it, Glyphs::Serif, bold, italic));
                    else
                        symbol.append(*it);
                }
            }
        }
    }
    renderText(symbol);
}

void Equation::renderFunction(int exponent)
{
    Q_ASSERT(isSymbolic());
    giac::gen arg(getFeuille());
    bool noParens = isElementaryFunction() &&
            (isNumber(arg) || isIdentifier(arg) || isSommet(arg, giac::at_abs));
    QString symbol = sommetName();
    QString digits("");
    if (symbol.length() == 1)
        symbol = QString(Glyphs::letterToMath(*symbol.begin(), Glyphs::Serif, false, true));
    if (exponent > 0)
        digits = Glyphs::digitsToSuperscript(QString::number(exponent));
    symbol += digits;
    symbol.append(Glyphs::functionApplicationSpace());
    QPointF where = renderText(symbol);
    Equation argument(arg, contextptr, fontFamily, fontSize);
    if (!noParens)
        argument.setBracketType(Parenthesis);
    argument.render();
    QPainter painter;
    painter.begin(this);
    painter.drawPicture(where, argument);
}

void Equation::renderModulo()
{
    Equation valueSymbol(*expression._MODptr, contextptr, fontFamily, fontSize);
    Equation modulusSymbol(*(expression._MODptr + 1), contextptr, fontFamily, fontSize);
    valueSymbol.render();
    modulusSymbol.setBracketType(Parenthesis);
    modulusSymbol.render();
    QPainter painter;
    painter.begin(this);
    QPointF where(0.0,0.0);
    painter.drawPicture(where, valueSymbol);
    where.setX(where.x() + valueSymbol.width());
    where = renderText(" (mod ", where);
    painter.drawPicture(where, modulusSymbol);
    where.setX(where.x() + modulusSymbol.width());
    renderText(")", where);
}

void Equation::renderMap()
{

}

void Equation::renderVector()
{
    switch (subType())
    {
    case giac::_SEQ__VECT:

        break;
    case giac::_SET__VECT:

        break;
    case giac::_ASSUME__VECT:

        break;
    case giac::_POLY1__VECT:

        break;
    case giac::_MATRIX__VECT:

        break;
    case giac::_LINE__VECT:

        break;
    case giac::_LIST__VECT:

        break;
    default:

        ;
    }
}

void Equation::renderSymbolic()
{
    Q_ASSERT(isSymbolic());
    if (isNegative())
    {

    }
    else if (isSommet(giac::at_plus))
        renderSequence(toVector(getFeuille()), paddedString("+"), true);
    else if (isSommet(giac::at_prod))
        renderSequence(toVector(getFeuille()), Glyphs::invisibleTimesSpace());
    else if (isSommet(giac::at_inv))
        renderFraction(giac::gen(1), getFeuille());
    else if (isSommet(giac::at_pow))
        renderPower(firstOperand(), secondOperand());
    else if (isSommet(giac::at_rootof))
    {

    }
    else if (isSommet(giac::at_interval))
        renderSequence(toVector(getFeuille()), paddedString(Glyphs::twoDotLeaderSymbol()));
    else if (isSommet(giac::at_union))
        renderSequence(assocOperands(), paddedString(Glyphs::unionSymbol()));
    else if (isSommet(giac::at_intersect))
        renderSequence(assocOperands(), paddedString(Glyphs::intersectionSymbol()));
    else if (isSommet(giac::at_compose))
        renderSequence(assocOperands(), paddedString(Glyphs::ringOperatorSymbol()));
    else if (isSommet(giac::at_minus))
        renderSequence(toVector(getFeuille()), paddedString(Glyphs::setMinusSymbol()));
    else if (isSommet(giac::at_composepow))
    {

    }
    else
    {

    }
}

void Equation::renderSequence(const giac::vecteur &operands, const QString &separator, bool isAddition)
{

}

void Equation::renderFraction(const giac::gen &numerator, const giac::gen &denominator)
{

}

void Equation::renderPower(const giac::gen &base, const giac::gen &exponent)
{

}

void Equation::renderRoot(const giac::gen &arg, int degree)
{

}
