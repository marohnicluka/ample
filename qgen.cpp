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

#include "qgen.h"

const QGen QGen::typeError(const QString &message)
{
    giac::gen err(giac::gentypeerr(message.toStdString()));
    return QGen(err, contextPtr);
}

QString QGen::paddedString(QString str, bool padLeft, bool padRight)
{
    QChar space = MathGlyphs::mediumMathSpace();
    QString padded(str);
    if (padLeft)
        padded.prepend(space);
    if (padRight)
        padded.append(space);
    return padded;
}

QString QGen::toString()
{
    std::stringstream ss;
    ss << (giac::gen)*this;
    return QString(ss.str().data());
}

QString QGen::numberToSuperscript(int integer, bool parens)
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

QString QGen::numberToSubscript(int integer, bool parens)
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

bool QGen::hasMinusSign()
{
    return  (isSymbolic() && isSommet(giac::at_neg)) ||
            (isInteger() && integerValue() < 0) ||
            (isApproximate() && doubleValue() < 0) ||
            (isFraction() && !giac::is_positive(fractionNumerator(),contextPtr));
}

bool QGen::isElementaryFunction() {
    return  isSommet(giac::at_exp) || isSommet(giac::at_ln) || isSommet(giac::at_log10) ||
            isSommet(giac::at_sin) || isSommet(giac::at_cos) || isSommet(giac::at_tan) ||
            isSommet(giac::at_sec) || isSommet(giac::at_csc) || isSommet(giac::at_cot) ||
            isSommet(giac::at_asin) || isSommet(giac::at_acos) || isSommet(giac::at_atan) ||
            isSommet(giac::at_acot) || isSommet(giac::at_sinh) || isSommet(giac::at_cosh) ||
            isSommet(giac::at_tanh) || isSommet(giac::at_asinh) ||
            isSommet(giac::at_acosh) || isSommet(giac::at_atanh);
}

bool QGen::isAssociativeOperation()
{
    return  isSummation() || isProduct() || isUnion() || isIntersection() ||
            isConjunction() || isDisjunction() || isExclusiveOr() || isComposition();
}

bool QGen::isUnaryOperation()
{
    return  isNegation() || isMinusSign() || isDerivative() || isFactorial() ||
            isIntegral() || isReciprocal() || isIncrement() || isDecrement();
}

bool QGen::isBinaryOperation()
{
    return  isPower() || isEqual() || isDifferent() || isLessThan() || isGreaterThan() ||
            isLessThanOrEqualTo() || isGreaterThanOrEqualTo() || isSetMinus() ||
            isDefinition() || isCompositionPower() || isCrossProduct();
}

bool QGen::isRelation()
{
    return  isEquality() || isLessThan() || isGreaterThan() ||
            isLessThanOrEqualTo() || isGreaterThanOrEqualTo();
}

QGen::Vector QGen::assocOperands()
{
    Vector operands;
    QGen feuille = getFeuille();
    if (!feuille.isVector())
        operands.push_back(feuille);
    else
    {
        Vector::iterator it;
        for (int i = 0 ; i < feuille.vectorLength(); ++i) {
            QGen operand = feuille.getOperand(i);
            if (operand.isSommet(getSommet())) {
                Vector subOperands = operand.assocOperands();
                for (it = subOperands.begin(); it != subOperands.end(); ++it)
                    operands.push_back(*it);
            }
            else operands.push_back(operand);
        }
    }
    return operands;
}

QGen QGen::getOperand(int n)
{
    QGen operands(getFeuille(), contextPtr);
    if (operands.isVector())
    {
        Q_ASSERT(operands.vectorLength() > n);
        QGen operand(operands.toVector().at(n), contextPtr);
        return operand;
    }
    Q_ASSERT(n == 0);
    return operands;
}

void QGen::render()
{
    if (isString())
        renderString();
    else if (isExplicitNumber())
        renderNumber();
    else if (isIdentifier())
        renderIdentifier();
    else if (isFunction())
        renderFunction();
    else if (isModular())
        renderModulo();
    else if (isMap())
        renderMap();
    else if (isVector())
        renderVector();
    else if (isSymbolic())
        renderSymbolic();
    else
        putText(toString());
}

QPointF QGen::putText(QString text, QPointF where)
{
    QPainter painter();
    painter.drawText(QPointF(where.x()-glyphs.leftBearing(*text.begin()),where.y()), text);
    return QPointF(where.x() + glyphs.boundingRect(text).width(), where.y());
}

QPicture QGen::renderString()
{
    Q_ASSERT(isString());
    QString text(stringValue());
    text.prepend(MathGlyphs::leftDoubleQuotationMarkSymbol());
    text.append(MathGlyphs::rightDoubleQuotationMarkSymbol());
    putText(text);
}

QPicture QGen::renderNumber()
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
            text.append(paddedString(MathGlyphs::multiplicationSignSymbol()));
            QString exponentDigits = glyphs.digitsToSuperscript(exponent);
            QString sign = negativeExponent ? QString(MathGlyphs::superscriptMinusSignSymbol()) : "";
            text.append("10" + sign + exponentDigits);
        }
        if (isNegative())
            text.prepend(MathGlyphs::minusSignSymbol());
        putText(text);
    }
}

QPicture QGen::renderIdentifier()
{
    Q_ASSERT(isIdentifier());
    giac::identificateur ident(*expression._IDNTptr);
    QString symbol;
    if (ident == giac::_IDNT_infinity())
        symbol = QString(MathGlyphs::infinitySymbol());
    else if (ident == giac::_IDNT_undef())
        symbol = tr("undefined");
    else
    {
        QString text = toString();
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
            if (glyphs.isGreekLetter(text, greekLetter))
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
    putText(symbol);
}

QPicture QGen::renderFunction(int exponent)
{
    Q_ASSERT(isSymbolic());
    giac::gen arg(getFeuille());
    bool noParens = isElementaryFunction() &&
            (isExplicitNumber(arg) || isIdentifier(arg) || isSommet(argiac::at_abs));
    QString symbol = sommetName();
    QString digits("");
    if (symbol.length() == 1)
        symbol = QString(MathGlyphs::letterToMath(*symbol.begin(), MathGlyphs::Serif, false, true));
    if (exponent > 0)
        digits = MathGlyphs::digitsToSuperscript(QString::number(exponent));
    symbol += digits;
    symbol.append(MathGlyphs::functionApplicationSpace());
    QPointF where = putText(symbol);
    QGen argument(arcontextptr, fontFamily, fontSize);
    if (!noParens)
        argument.setBracketType(Parenthesis);
    argument.render();
    QPainter painter(this);
    painter.drawPicture(where, argument);
}

QPicture QGen::renderModulo()
{
    QGen valueSymbol(*expression._MODptr, contextPtr, fontFamily, fontSize);
    QGen modulusSymbol(*(expression._MODptr + 1), contextPtr, fontFamily, fontSize);
    valueSymbol.render();
    modulusSymbol.setBracketType(Parenthesis);
    modulusSymbol.render();
    QPointF where(0.0,0.0);
    painter.drawPicture(where, valueSymbol);
    where.setX(where.x() + valueSymbol.width());
    where = putText(" (mod ", where);
    painter.drawPicture(where, modulusSymbol);
    where.setX(where.x() + modulusSymbol.width());
    putText(")", where);
}

QPicture QGen::renderMap()
{

}

QPicture QGen::renderVector()
{
    Q_ASSERT(isVector());
    if (isPolynomialVector())
    {

    }
    else if (isMatrixVector() || isMatrix())
    {

    }
    else if (isAssumeVector())
    {

    }
    else
    {

    }
}

QPicture QGen::renderSymbolic()
{
    Q_ASSERT(isSymbolic());
}

QPicture QGen::renderUnaryOperation()
{

}

QPicture QGen::renderBinaryOperation()
{

}

QPicture QGen::renderAssociativeOperation()
{

}

QPicture QGen::renderFraction(const QGen &numerator, const QGen &denominator)
{

}

QPicture QGen::renderPower(const QGen &base, const QGen &exponent)
{

}

QPicture QGen::renderRoot(const QGen &argument, int degree)
{

}
