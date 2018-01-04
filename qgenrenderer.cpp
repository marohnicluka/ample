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
#include <QRegularExpression>
#include <qmath.h>
#include "qgenrenderer.h"

#define FONT_SIZE_SCALING_FACTOR 1.71526586621

QGenRenderer::QGenRenderer(const QString &family, int fontSize)
{
    fontFamily = family;
    for (int i = -2; i < 2; ++i)
        fontSizes << qRound(fontSize * pow(FONT_SIZE_SCALING_FACTOR, i));
    renderFontBold = false;
    renderFontItalic = false;
}

QPicture QGenRenderer::render(const QGen &g, int alignment) const
{

}

QString QGenRenderer::paddedText(const QString &text, MathPadding padding, bool padLeft, bool padRight)
{
    QChar space;
    QString padded(text);
    switch (padding)
    {
    case Hair:
        space = MathGlyphs::hairSpace();
        break;
    case Thin:
        space = MathGlyphs::thinSpace();
        break;
    case Medium:
        space = MathGlyphs::mediumSpace();
        break;
    case Thick:
        space = MathGlyphs::thickSpace();
        break;
    }
    if (padLeft)
        padded.prepend(space);
    if (padRight)
        padded.append(space);
    return padded;
}

QString QGenRenderer::quotedText(const QString &text)
{
    QString quoted(text);
    quoted.prepend(MathGlyphs::leftDoubleQuotationMark());
    quoted.append(MathGlyphs::rightDoubleQuotationMark());
    return quoted;
}

QString QGenRenderer::numberToSuperscriptText(int integer, bool parens)
{
    bool neg = integer < 0;
    QString digits = QString::number(neg ? -integer : integer);
    QString raised = MathGlyphs::digitsToSuperscript(digits);
    if (neg)
        raised.prepend(MathGlyphs::superscriptMinus());
    if (parens)
    {
        raised.prepend(MathGlyphs::superscriptLeftParenthesis());
        raised.append(MathGlyphs::superscriptRightParenthesis());
    }
    return raised;
}

QString QGenRenderer::numberToSubscriptText(int integer, bool parens)
{
    bool neg = integer < 0;
    QString digits = QString::number(neg ? -integer : integer);
    QString raised = MathGlyphs::digitsToSubscript(digits);
    if (neg)
        raised.prepend(MathGlyphs::subscriptMinus());
    if (parens)
    {
        raised.prepend(MathGlyphs::subscriptLeftParenthesis());
        raised.append(MathGlyphs::subscriptRightParenthesis());
    }
    return raised;
}

QString QGenRenderer::identifierStringToUnicode(const QString &text, bool bold, bool italic)
{
    QString symbol;
    QChar greekLetter;
    if (MathGlyphs::getGreekLetter(text, greekLetter))
        symbol = MathGlyphs::letterToMath(greekLetter, MathGlyphs::Greek, bold, italic);
    else
    {
        QString::const_iterator it;
        for (it = text.begin(); it != text.end(); ++it)
        {
            if (it->isLetter())
                symbol.append(MathGlyphs::letterToMath(*it, MathGlyphs::Serif, bold, italic));
            else
                symbol.append(*it);
        }
    }
    return symbol;
}

const QFont &QGenRenderer::getFont(int fontSizeLevel)
{
    int level = 2 + (int)qMin(qMax(fontSizeLevel, -2), 1);
    QFont::Weight weight = renderFontBold ? QFont::Bold : QFont::Normal;
    int pointSize = fontSizes.at(level);
    for (QList<QFont>::const_iterator it = fonts.begin(); it != fonts.end(); ++it)
    {
        if (it->pointSize() == pointSize && it->weight() == weight && it->italic() == renderFontItalic)
            return *it;
    }
    fonts.append(QFont(fontFamily, pointSize, weight, renderFontItalic));
    return fonts.back();
}

void QGenRenderer::render(Display &dest, const QGen &g, int sizeLevel)
{
    fontSizeLevelStack.push(sizeLevel);
    if (g.isString())
        renderText(dest, quotedText(g.stringValue()));
    else if (g.isConstant())
        renderNumber(dest, g);
    else if (g.isIdentifier())
        renderIdentifier(dest, g);
    else if (g.isModular())
        renderModular(dest, g);
    else if (g.isMap())
        renderMap(dest, g);
    else if (g.isVector())
        renderVector(dest, g.toVector());
    else if (g.isSymbolic())
        renderSymbolic(dest, g);
    else
        renderText(dest, g.toString());
    fontSizeLevelStack.pop();
}

QGenRenderer::Display QGenRenderer::renderNormal(const QGen &g)
{
    Display display;
    render(display, g, fontSizeLevel());
    return display;
}

QGenRenderer::Display QGenRenderer::renderSmaller(const QGen &g)
{
    Display display;
    render(display, g, smallerFontSizeLevel());
    return display;
}

QGenRenderer::Display QGenRenderer::renderLarger(const QGen &g)
{
    Display display;
    render(display, g, largerFontSizeLevel());
    return display;
}

void QGenRenderer::renderDisplay(Display &dest, const Display &source, QPointF where)
{
    QPainter painter(&dest);
    painter.drawPicture(where, source);
}

void QGenRenderer::renderDisplayAndAdvance(Display &dest, const Display &source, QPointF &penPoint)
{
    renderDisplay(dest, source, penPoint);
    movePenPointX(penPoint, source.advance());
}

qreal QGenRenderer::renderText(Display &dest, const QString &text,
                              int relativeFontSizeLevel, QPointF where, QRectF *boundingRect)
{
    QPainter painter(&dest);
    QFont font = getFont(fontSizeLevel() + relativeFontSizeLevel);
    QFontMetricsF fontMetrics(font);
    painter.setFont(font);
    painter.drawText(where, text);
    if (boundingRect != Q_NULLPTR)
    {
        *boundingRect = fontMetrics.tightBoundingRect(text);
        boundingRect->translate(where);
    }
    return textWidth(text);
}

void QGenRenderer::renderTextAndAdvance(Display &dest, const QString &text, QPointF &penPoint)
{
    movePenPointX(penPoint, renderText(dest, text, 0, penPoint));
}

qreal QGenRenderer::textWidth(const QString &text, int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.width(text);
}

QRectF QGenRenderer::textTightBoundingRect(const QString &text, int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.tightBoundingRect(text);
}

qreal QGenRenderer::fontHeight(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.height();
}

qreal QGenRenderer::fontAscent(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.ascent();
}

qreal QGenRenderer::fontDescent(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.descent();
}

qreal QGenRenderer::fontXHeight(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.xHeight();
}

qreal QGenRenderer::fontMidLine(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return (fontMetrics.xHeight() / 2 + fontMetrics.strikeOutPos()) / 2;
}

qreal QGenRenderer::fontLeading(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.leading();
}

qreal QGenRenderer::lineWidth(int relativeFontSizeLevel)
{
    return textTightBoundingRect(MathGlyphs::horizontalLineExtension(), relativeFontSizeLevel).height();
}

void QGenRenderer::renderNumber(Display &dest, const QGen &g, QPointF where)
{
    QPointF penPoint(where);
    QGen gAbs = (g.isNegativeConstant() ? -g : g), numerator, denominator, realPart, imaginaryPart;
    if (g.isNegativeConstant())
    {
        renderTextAndAdvance(dest, MathGlyphs::minus(), penPoint);
        dest.setPriority(QGen::MultiplicationPriority);
    }
    if (gAbs.isFraction(numerator, denominator)) {
        renderFraction(dest, numerator, denominator, penPoint);
        dest.setPriority(QGen::DivisionPriority);
        dest.setGrouped(true);
    }
    else if (gAbs.isComplex(realPart, imaginaryPart))
    {
        Display realPartDisplay = renderNormal(realPart);
        QGen imAbs = (imaginaryPart.isNegativeConstant() ? -imaginaryPart : imaginaryPart);
        Display imAbsDisplay = renderNormal(imAbs);
        renderDisplayAndAdvance(dest, realPartDisplay, penPoint);
        QChar operatorChar = imaginaryPart.isNegativeConstant() ? MathGlyphs::minus() : '+';
        renderTextAndAdvance(dest, paddedText(operatorChar), penPoint);
        renderDisplayAndAdvance(dest, imAbsDisplay, penPoint);
        QString imaginaryUnit("i");
        imaginaryUnit.prepend(MathGlyphs::thinSpace());
        renderText(dest, imaginaryUnit, 0, penPoint);
        dest.setPriority(QGen::AdditionPriority);
    }
    else
    {
        QString text = gAbs.toString();
        int index;
        if ((index = text.indexOf("e")) >= 0)
        {
            QString mantissa = text.left(index);
            QString exponent = text.mid(index + 1);
            bool hasNegativeExponent = exponent.startsWith("-");
            if (hasNegativeExponent)
                exponent = exponent.mid(1);
            text = mantissa;
            text.append(paddedText(MathGlyphs::times()));
            QString exponentDigits = MathGlyphs::digitsToSuperscript(exponent);
            if (hasNegativeExponent)
                exponentDigits.prepend(MathGlyphs::superscriptMinus());
            text.append("10" + exponentDigits);
            dest.setPriority(QGen::MultiplicationPriority);
        }
        renderText(dest, text, 0, penPoint);
    }
}

void QGenRenderer::renderIdentifier(Display &dest, const QGen &g, QPointF where)
{
    Q_ASSERT(g.isIdentifier());
    QString text, symbol, index;
    if (g.isInfinityIdentifier())
        symbol = QString(MathGlyphs::infinity());
    else if (g.isUndefIdentifier())
        symbol = "undefined";
    else if ((text = g.toString()).startsWith("_"))
        renderLeadingUnderscoreIdentifier(dest, g, where);
    else
    {
        bool italic = !(g.isPi() || g.isEulerNumber() || g.isEulerMascheroniConstant() || g.isImaginaryUnit());
        bool bold = false;
        QRegularExpressionMatch rxMatch;
        if (g.isEulerMascheroniConstant())
            text = "gamma";
        else if (g.isPlaceholderIdentifier() || g.isDoubleLetterIdentifier())
        {
            bold = true;
            if (italic = g.isPlaceholderIdentifier())
                text.chop(1);
            text = text.mid(1).trimmed();
        }
        else if ((rxMatch = QRegularExpression("_[0-9]+$").match(text)).hasMatch())
        {
            int i = rxMatch.capturedStart();
            index = text.mid(i + 1);
            text = text.left(i);
        }
        symbol = identifierStringToUnicode(text, bold, italic);
        if (index.length() > 0)
            symbol.append(MathGlyphs::digitsToSubscript(index));
    }
    renderText(dest, symbol, 0, where);
}

void QGenRenderer::renderLeadingUnderscoreIdentifier(Display &dest, const QGen &g, QPointF where)
{
    QString text = g.toString(), subscript;
    bool italic = true;
    if (text.endsWith("_"))
    {
        text = text.mid(1, -2);
        if (text == "NA")
        {
            text = "N";
            subscript = "A";
        }
        else if (text == "Vm")
            text = QString("V") + MathGlyphs::subscriptSmallLetterM();
        else if (QRegularExpression("^Std[TP]$").match(text).hasMatch())
        {
            text = text.right(1);
            subscript = "std";
        }
        else if (text == "hbar")
        {
            text = MathGlyphs::smallLetterHWithStroke();
            setRenderingItalic(true);
        }
        else if (text == "me")
            text = QString("m") + MathGlyphs::subscriptSmallLetterE();
        else if (text == "qme")
        {
            text = QString("q/m") + MathGlyphs::subscriptSmallLetterE();
            dest.setPriority(QGen::MultiplicationPriority);
        }
        else if (text == "mp")
            text = QString("m") + MathGlyphs::subscriptSmallLetterP();
        else if (text == "mpme")
        {
            text = QString("m") + MathGlyphs::subscriptSmallLetterP() +
                    QString("/m") + MathGlyphs::subscriptSmallLetterE();
            dest.setPriority(QGen::MultiplicationPriority);
        }
        else if (text == "Rinfinity")
        {
            text = "R";
            subscript = MathGlyphs::infinity();
        }
        else if (QRegularExpression("^.(Sun|Earth)$").match(text).hasMatch())
        {
            subscript = text.mid(1);
            text = text.left(1);
        }
        if (text == "lambdac")
            text = "lambda";
        else if (text == "angl")
        {
            text = QString("180") + MathGlyphs::degree();
            dest.setPriority(QGen::MultiplicationPriority);
        }
        else if (text == "twopi")
        {
            text = QString("2") + QString(MathGlyphs::thinSpace()) +
                    QString(MathGlyphs::smallLetterPi()) + MathGlyphs::thickSpace() + QString("rad");
            dest.setPriority(QGen::MultiplicationPriority);
        }
        else if (text == "c3")
            text = "b";
        else if (text == "kq")
        {
            text = "k/q";
            dest.setPriority(QGen::MultiplicationPriority);
        }
        else if (text == "epsilon0q")
            text = MathGlyphs::smallLetterEpsilon() + MathGlyphs::digitsToSubscript("0") + "/q";
        else if (text == "qepsilon0")
            text = QString("q") + MathGlyphs::invisibleTimesSpace() +
                    MathGlyphs::smallLetterEpsilon() + MathGlyphs::digitsToSubscript("0");
        else if (text == "epsilonsi")
        {
            text.chop(1);
            subscript = "Si";
        }
        else if (text == "epsilonox") {
            text.chop(2);
            subscript = QString("Si") + MathGlyphs::invisibleSeparatorSpace() +
                    QString("O") + MathGlyphs::digitsToSubscript("2");
        }
        else if (QRegularExpression("^[a-z]+[0BN]$").match(text).hasMatch())
        {
            subscript = text.right(1);
            text.chop(1);
        }
    }
    else
    {
        italic = false;
        text = text.mid(1);
        if (text == "deg")
            text = MathGlyphs::degree();
        else if (text == "degreeF")
            text = MathGlyphs::degreeFahrenheit();
        else if (text == "Ohm")
            text = "Omega";
    }
    QPointF penPoint(where);
    QString symbol = identifierStringToUnicode(text, false, italic);
    renderTextAndAdvance(dest, symbol, penPoint);
    setRenderingItalic(false);
    if (subscript.length() > 0)
    {
        movePenPointY(penPoint, fontXHeight(-1));
        renderText(dest, subscript, -1, penPoint);
    }
}

void QGenRenderer::renderModular(Display &dest, const QGen &g, QPointF where)
{
    QGen value, modulus;
    Q_ASSERT(g.isModular(value, modulus));
    Display valueDisplay = renderNormal(value), modulusDisplay = renderNormal(modulus), display;
    QPointF penPoint(0, 0);
    renderTextAndAdvance(display, "mod ", penPoint);
    renderDisplayAndAdvance(display, modulusDisplay, penPoint);
    penPoint = where;
    int priority = QGen::ModularPriority;
    qreal advance = renderDisplayWithPriority(dest, valueDisplay, priority, penPoint) + textWidth(" ");
    movePenPointX(penPoint, advance);
    renderDisplayWithParentheses(dest, display, penPoint);
    dest.setPriority(priority);
}

void QGenRenderer::renderUnary(Display &dest, const QGen &g, QPointF where)
{
    QPointF penPoint(where);
    QString prefix;
    QGen argument = g.unaryFunctionArgument();
    Display argumentDisplay;
    int priority = g.operatorPriority();
    if (priority == QGen::UnaryPriority)
    {
        if (g.isMinusOperator())
            prefix = MathGlyphs::minus();
        else if (g.isNegationOperator())
            prefix = MathGlyphs::notSign();
        else if (g.isIncrementOperator())
            prefix = "++";
        else if (g.isDecrementOperator())
            prefix = "--";
        else
        {
            if (g.isRealPartOperator())
                prefix = MathGlyphs::letterToMath('R', MathGlyphs::Fraktur, false, false);
            else if (g.isImaginaryPartOperator())
                prefix = MathGlyphs::letterToMath('I', MathGlyphs::Fraktur, false, false);
            else if (g.isTraceOperator())
                prefix = "tr";
            else Q_ASSERT(false); // unreachanble
            prefix.append(MathGlyphs::functionApplicationSpace());
        }
        renderTextAndAdvance(dest, prefix, penPoint);
        argumentDisplay = renderNormal(argument);
        renderDisplayWithPriority(dest, argumentDisplay, priority, penPoint);
    }
    else if (g.isReciprocalOperator())
    {

    }
    else if (g.isDerivativeOperator(true))
    {
        int degree = 1;
        while (argument.isDerivativeOperator(true))
        {
            ++degree;
            argument = g.unaryFunctionArgument();
        }
        argumentDisplay = renderNormal(argument);
        movePenPointX(penPoint, renderDisplayWithPriority(dest, argumentDisplay, priority, penPoint));
        QString suffix;
        switch (degree)
        {
        case 1:
            suffix = MathGlyphs::prime();
            break;
        case 2:
            suffix = MathGlyphs::doublePrime();
            break;
        case 3:
            suffix = MathGlyphs::triplePrime();
            break;
        default:
            suffix = MathGlyphs::digitsToSuperscript(QString::number(degree));
            suffix.prepend(MathGlyphs::superscriptLeftParenthesis());
            suffix.append(MathGlyphs::superscriptRightParenthesis());
        }
        qreal yOffset = qMax(0.0, argumentDisplay.ascent() - fontAscent());
        movePenPointY(penPoint, -yOffset);
        renderText(dest, suffix, 0, penPoint);
    }
    else if (priority == QGen::ExponentiationPriority)
    {
        argumentDisplay = renderNormal(argument);
        Display operatorDisplay;
        if (g.isTranspositionOperator() || g.isFactorialOperator())
        {
            movePenPointX(penPoint, renderDisplayWithPriority(dest, argumentDisplay, priority, penPoint));
            if (g.isTranspositionOperator())
            {
                renderText(operatorDisplay, "T", -1, penPoint);
                qreal yOffset = fontXHeight() + qMax(0.0, argumentDisplay.ascent() - fontAscent());
                movePenPointY(penPoint, -yOffset);
                renderDisplay(dest, operatorDisplay, penPoint);
            }
            else if (g.isFactorialOperator())
                renderText(dest, "!", 0, penPoint);
        }
        else if (g.isComplexConjugateOperator())
            renderDisplayWithAccent(dest, argumentDisplay, AccentType::Bar, penPoint);
        else Q_ASSERT(false); // unreachanble
    }
    dest.setPriority(priority);
}

void QGenRenderer::renderBinary(Display &dest, const QGen &g, QPointF where)
{
    QPointF penPoint(where);
    int priority = g.operatorPriority();
    if (priority = QGen::ExponentiationPriority)
    {
        bool withCircle = g.isHadamardPowerOperator() || g.isFunctionalPowerOperator();
        renderSuperscript(dest, g.firstOperand(), g.secondOperand(), penPoint, withCircle);
    }
    else
    {
        QGen left = g.firstOperand(), right = g.secondOperand();
        QChar op;
        bool hasConstant;
        if (g.isEquation())
            op = '=';
        else if (g.isEqualOperator())
            op = MathGlyphs::questionedEqualTo();
        else if (g.isElementOperator())
            op = MathGlyphs::elementOf();
        else if (g.isNotEqualOperator())
            op = MathGlyphs::notEqualTo();
        else if (g.isLessThanOperator())
            op = '<';
        else if (g.isGreaterThanOperator())
            op = '>';
        else if (g.isLessThanOrEqualOperator())
            op = MathGlyphs::lessThanOrEqualTo();
        else if (g.isGreaterThanOrEqualOperator())
            op = MathGlyphs::greaterThanOrEqualTo();
        else if (g.isAssignmentOperator())
            op = MathGlyphs::equalToByDefinition();
        else if (g.isArrayAssignmentOperator())
            op = MathGlyphs::leftwardsArrow();
        else if (g.isCrossProductOperator())
            op = MathGlyphs::vectorOrCrossProduct();
        else if (g.isSetDifferenceOperator())
            op = MathGlyphs::setMinus();
        else if (g.isHadamardDivisionOperator())
            op = MathGlyphs::circledDivisionSlash();
        else if (g.isHadamardDifferenceOperator())
            op = MathGlyphs::minus();
        else if (g.isUnitApplicationOperator(hasConstant))
            op = hasConstant ? MathGlyphs::multiplicationDot() : MathGlyphs::thickSpace();
        else if (g.isIntervalOperator())
            op = MathGlyphs::twoDotLeader();
        else if (g.isMappingOperator())
        {
            int n = 0;
            while (n < g.operandCount() && !g.nthOperand(n).isZero())
                n++;
            left = g.unaryFunctionArgument();
            left.resizeVector(n);
            if (left.length() == 1)
                left = g.firstOperand();
            right = g.lastOperand();
        }
        else
            Q_ASSERT(false); // unreachable
        Display leftDisplay = renderNormal(left);
        Display rightDisplay = renderNormal(right);
        movePenPointX(penPoint, renderDisplayWithPriority(dest, leftDisplay, priority, penPoint));
        renderTextAndAdvance(dest, paddedText(op), penPoint);
        renderDisplayWithPriority(dest, rightDisplay, priority, penPoint);
    }
    dest.setPriority(priority);
}

void QGenRenderer::renderAssociative(Display &dest, const QGen &g,
                                              const QGen::Vector operands, QPointF where)
{
    QPointF penPoint(where);
    QString opStr;
    if (g.isProductOperator())
    {
        QGen numerator, denominator;
        if (g.isRationalExpression(numerator, denominator))
        {
            renderFraction(dest, numerator, denominator, where);
            return;
        }
    }
    else if (g.isSumOperator() || g.isHadamardSumOperator())
        opStr = paddedText("+");
    else if (g.isUnionOperator())
        opStr = paddedText(MathGlyphs::setUnion());
    else if (g.isIntersectionOperator())
        opStr = paddedText(MathGlyphs::setIntersection());
    else if (g.isConjunctionOperator())
        opStr = paddedText(MathGlyphs::logicalAnd());
    else if (g.isDisjunctionOperator())
        opStr = paddedText(MathGlyphs::logicalOr());
    else if (g.isExclusiveOrOperator())
        opStr = paddedText(MathGlyphs::logicalXor());
    else if (g.isComposeOperator() || g.isHadamardProductOperator())
        opStr = paddedText(MathGlyphs::ringOperator());
    else
        opStr = paddedText("??");
    Display display;
    for (int i = 0; i < operands.length(); ++i)
    {
        QGen operand = operands.at(i);
        bool parenthesize = true;

        display = renderNormal(operand);
        renderDisplayAndAdvance(dest, display, penPoint);
        if (i < operands.length() - 1)
            renderTextAndAdvance(dest, opStr, penPoint);
    }
}

void QGenRenderer::renderFraction(Display &dest, const QGen &numerator, const QGen &denominator, QPointF where)
{
    QPointF penPoint(where);
    Display numeratorDisplay = renderNormal(numerator);
    Display denominatorDisplay = renderNormal(denominator);
    qreal width = qMax(numeratorDisplay.width(), denominatorDisplay.width());
    qreal padding = fontLeading() + 0.5;
    movePenPointY(penPoint, -fontMidLine() + 0.5);
    renderHorizontalLine(dest, penPoint, width);
    QPointF numeratorPenPoint(penPoint), denominatorPenPoint(penPoint);
    movePenPointX(numeratorPenPoint, numeratorDisplay.leftBearing() + (width - numeratorDisplay.width()) / 2.0);
    movePenPointY(numeratorPenPoint, -(padding + numeratorDisplay.descent()));
    renderDisplay(dest, numeratorDisplay, penPoint);
    movePenPointX(denominatorPenPoint, denominatorDisplay.leftBearing() + (width - denominatorDisplay.width()) / 2.0);
    movePenPointY(denominatorPenPoint, padding + denominatorDisplay.ascent());
    renderDisplay(dest, denominatorDisplay, penPoint);
}

void QGenRenderer::renderSuperscript(Display &dest, const QGen &base, const QGen &exponent,
                                     int priority, QPointF where, bool withCircle)
{
    QPointF penPoint(where);
    Display baseDisplay, exponentDisplay;
    baseDisplay = renderNormal(base);
    exponentDisplay = renderSmaller(exponent);
    movePenPointX(penPoint, renderDisplayWithPriority(dest, baseDisplay, priority, penPoint));
    qreal verticalOffset = qMax(0.0, baseDisplay.ascent() - fontAscent());
    movePenPointXY(penPoint, exponentDisplay.leftBearing(), -(fontXHeight() + verticalOffset));
    if (withCircle)
        movePenPointX(penPoint, renderText(dest, MathGlyphs::ringOperator(), -1, penPoint));
    renderDisplay(dest, exponentDisplay, penPoint);
}

void QGenRenderer::renderSubscript(Display &dest, const QGen &base, const QGen &subscript, int priority, QPointF where)
{
    QPointF penPoint(where);
    Display baseDisplay, subscriptDisplay;
    baseDisplay = renderNormal(base);
    subscriptDisplay = renderSmaller(subscript);
    movePenPointX(penPoint, renderDisplayWithPriority(dest, baseDisplay, priority, penPoint));
    qreal verticalOffset = qMax(0.0, baseDisplay.descent() - fontDescent());
    movePenPointXY(penPoint, subscriptDisplay.leftBearing(), fontXHeight(-1) + verticalOffset);
    renderDisplay(dest, subscriptDisplay, penPoint);
}

void QGenRenderer::renderDisplayWithRadical(Display &dest, const Display &source, QPointF where)
{
    QRectF rect = textTightBoundingRect(MathGlyphs::squareRoot());
    qreal rWidth = rect.width(), rHeight = rect.height(), rDescent = rect.y() + rHeight, rBearing = -rect.x();
    qreal radicandWidth = source.totalWidth(), radicandHeight = qMax(source.height() + linePadding(), rHeight);
    qreal fY = qMax(1.0, (radicandHeight + lineWidth() / 3) / rHeight), fX = qPow(fY, 0.2);
    renderDisplay(dest, source, QPointF(where.x() + rWidth * fX + source.leftBearing(), where.y()));
    QPainter painter(&dest);
    painter.setFont(getFont(fontSizeLevel()));
    renderHorizontalLine(painter, rWidth * fX - lineWidth() / 3, radicandHeight - source.descent(), radicandWidth);
    painter.translate(where.x(), where.y() + source.descent());
    painter.scale(fX, fY);
    painter.drawText(QPointF(rBearing, -rDescent), MathGlyphs::squareRoot());
}

void QGenRenderer::renderMap(Display &dest, const QGen &g, QPointF where)
{

}

void QGenRenderer::renderVector(Display &dest, const QGen::Vector &v, QPointF where)
{

}

void QGenRenderer::renderSymbolic(Display &dest, const QGen &g, QPointF where)
{

}

void QGenRenderer::renderBracketExtensionFill(QPainter &painter, const QChar &extension,
                                              qreal x, qreal yLower, qreal yUpper)
{
    painter.save();
    QRectF rect = textTightBoundingRect(extension);
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

qreal QGenRenderer::renderSingleBracket(QPainter &painter, qreal x, qreal halfHeight, QChar bracket, bool scaleX)
{
    QRectF rect = textTightBoundingRect(bracket);
    qreal f = qMax(1.0, halfHeight / -(1 + fontMidLine() + rect.y()));
    if (f > 0)
    {
        painter.save();
        painter.scale(scaleX ? qPow(f, 0.2) : 1.0, f);
        painter.drawText(QPointF(x, fontMidLine() * (f - 1) / f), bracket);
        painter.restore();
    }
    return textWidth(bracket);
}

qreal QGenRenderer::renderSingleFillBracket(QPainter &painter, qreal x, qreal halfHeight,
                                            QChar upperPart, QChar lowerPart, QChar extension, QChar singleBracket)
{
    QRectF rect = textTightBoundingRect(upperPart);
    qreal upperPartHeight = rect.height() - 2, upperPartDescent = rect.y() + rect.height();
    if (halfHeight < upperPartHeight)
        return renderSingleBracket(painter, x, halfHeight, singleBracket);
    qreal yLower = 0.5 - fontMidLine() + halfHeight;
    qreal yUpper = 0.5 - fontMidLine() - halfHeight;
    rect = textTightBoundingRect(lowerPart);
    qreal lowerPartHeight = rect.height() - 2, lowerPartDescent = rect.y() + rect.height();
    painter.drawText(QPointF(x, yUpper + upperPartHeight - upperPartDescent), upperPart);
    painter.drawText(QPointF(x, yLower - lowerPartDescent), lowerPart);
    renderBracketExtensionFill(painter, extension, x, yLower - lowerPartHeight, yUpper + upperPartHeight);
    return textWidth(upperPart);
}

qreal QGenRenderer::renderCurlyBracket(QPainter &painter, qreal x, qreal halfHeight, bool left)
{
    QChar upperPart = left ? MathGlyphs::leftCurlyBracketUpperHook() : MathGlyphs::rightCurlyBracketUpperHook();
    QChar middlePiece = left ? MathGlyphs::leftCurlyBracketMiddlePiece() : MathGlyphs::rightCurlyBracketMiddlePiece();
    QChar lowerPart = left ? MathGlyphs::leftCurlyBracketLowerHook() : MathGlyphs::rightCurlyBracketLowerHook();
    QChar extension = left ? MathGlyphs::leftCurlyBracketExtension() : MathGlyphs::rightCurlyBracketExtension();
    QChar upperHalf = left ? MathGlyphs::upperLeftCurlyBracketSection() : MathGlyphs::lowerLeftCurlyBracketSection();
    QChar lowerHalf = left ? MathGlyphs::lowerLeftCurlyBracketSection() : MathGlyphs::upperLeftCurlyBracketSection();
    QChar singleBracket = left ? '{' : '}';
    QRectF rect = textTightBoundingRect(upperPart);
    qreal upperPartHeight = rect.height() - 2, upperPartDescent = rect.y() + rect.height();
    rect = textTightBoundingRect(lowerPart);
    qreal lowerPartHeight = rect.height() - 2, lowerPartDescent = rect.y() + rect.height();
    rect = textTightBoundingRect(middlePiece);
    qreal middlePieceHeight = rect.height() - 2, middlePieceDescent = rect.y() + rect.height();
    rect = textTightBoundingRect(upperHalf);
    qreal halfBraceHeight = rect.height() - 2, halfBraceDescent = rect.y() + rect.height();
    qreal offset = fontXHeight() - fontMidLine();
    if (halfHeight >= fontXHeight() + middlePieceDescent + lowerPartHeight)
    {
        qreal upperPartY = 0.5 - halfHeight + upperPartHeight - fontMidLine();
        qreal lowerPartY = 0.5 + halfHeight - lowerPartHeight - fontMidLine();
        qreal y = 2 * offset;
        painter.drawText(QPointF(x, y - middlePieceDescent), middlePiece);
        painter.drawText(QPointF(x, upperPartY - upperPartDescent - 0.5), upperPart);
        painter.drawText(QPointF(x, lowerPartY + lowerPartHeight - lowerPartDescent), lowerPart);
        renderBracketExtensionFill(painter, extension, x, y - middlePieceHeight + 1, upperPartY);
        renderBracketExtensionFill(painter, extension, x, lowerPartY + 1, y);
        return textWidth(upperPart);
    }
    if (halfHeight >= halfBraceHeight)
    {
        qreal f = halfHeight / halfBraceHeight;
        painter.translate(x, 0.5 - fontMidLine());
        painter.scale(1.0, f);
        painter.drawText(QPointF(0.0, -halfBraceDescent - 0.5), upperHalf);
        painter.drawText(QPointF(0.0, halfBraceHeight - halfBraceDescent), lowerHalf);
        return textWidth(upperHalf);
    }
    return renderSingleBracket(painter, x, halfHeight, singleBracket);
}

qreal QGenRenderer::renderDisplayWithBrackets(Display &dest, const Display &source,
                                              BracketType leftBracketType, BracketType rightBracketType, QPointF where)
{
    qreal x = 0.0, halfHeight = qMax(source.ascent() - fontMidLine(), source.descent() + fontMidLine());
    QPainter painter(&dest);
    painter.setFont(getFont(fontSizeLevel()));
    painter.translate(where);
    switch (leftBracketType)
    {
    case Parenthesis:
        x += renderSingleFillBracket(painter, x, halfHeight,
                                     MathGlyphs::leftParenthesisUpperHook(),
                                     MathGlyphs::leftParenthesisLowerHook(),
                                     MathGlyphs::leftParenthesisExtension(),
                                     '(');
        break;
    case WhiteParenthesis:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::leftWhiteParenthesis(), true);
        break;
    case SquareBracket:
        x += renderSingleFillBracket(painter, x, halfHeight,
                                     MathGlyphs::leftSquareBracketUpperCorner(),
                                     MathGlyphs::leftSquareBracketLowerCorner(),
                                     MathGlyphs::leftSquareBracketExtension(),
                                     '[');
        break;
    case WhiteSquareBracket:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::leftWhiteSquareBracket());
        break;
    case CurlyBracket:
        x += renderCurlyBracket(painter, x, halfHeight, true);
        break;
    case WhiteCurlyBracket:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::leftWhiteCurlyBracket(), true);
        break;
    case FloorBracket:
        x += renderSingleFillBracket(painter, x, halfHeight,
                                     MathGlyphs::leftSquareBracketExtension(),
                                     MathGlyphs::leftSquareBracketLowerCorner(),
                                     MathGlyphs::leftSquareBracketExtension(),
                                     MathGlyphs::leftFloorBracket());
        break;
    case CeilingBracket:
        x += renderSingleFillBracket(painter, x, halfHeight,
                                     MathGlyphs::leftSquareBracketUpperCorner(),
                                     MathGlyphs::leftSquareBracketExtension(),
                                     MathGlyphs::leftSquareBracketExtension(),
                                     MathGlyphs::leftCeilingBracket());
        break;
    case AngleBracket:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::leftAngleBracket(), true);
        break;
    case StraightBracket:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::straightBracket());
        break;
    case DoubleStraightBracket:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::doubleStraightBracket());
        break;
    default:
        break;
    }
    renderDisplay(dest, source, QPointF(x, 0.0));
    x += source.advance();
    switch (rightBracketType)
    {
    case Parenthesis:
        x += renderSingleFillBracket(painter, x, halfHeight,
                                     MathGlyphs::rightParenthesisUpperHook(),
                                     MathGlyphs::rightParenthesisLowerHook(),
                                     MathGlyphs::rightParenthesisExtension(),
                                     ')');
        break;
    case WhiteParenthesis:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::rightWhiteParenthesis(), true);
        break;
    case SquareBracket:
        x += renderSingleFillBracket(painter, x, halfHeight,
                                     MathGlyphs::rightSquareBracketUpperCorner(),
                                     MathGlyphs::rightSquareBracketLowerCorner(),
                                     MathGlyphs::rightSquareBracketExtension(),
                                     ']');
        break;
    case WhiteSquareBracket:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::rightWhiteSquareBracket());
        break;
    case CurlyBracket:
        x += renderCurlyBracket(painter, x, halfHeight, false);
        break;
    case WhiteCurlyBracket:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::rightWhiteCurlyBracket(), true);
        break;
    case FloorBracket:
        x += renderSingleFillBracket(painter, x, halfHeight,
                                     MathGlyphs::rightSquareBracketExtension(),
                                     MathGlyphs::rightSquareBracketLowerCorner(),
                                     MathGlyphs::rightSquareBracketExtension(),
                                     MathGlyphs::rightFloorBracket());
        break;
    case CeilingBracket:
        x += renderSingleFillBracket(painter, x, halfHeight,
                                     MathGlyphs::rightSquareBracketUpperCorner(),
                                     MathGlyphs::rightSquareBracketExtension(),
                                     MathGlyphs::rightSquareBracketExtension(),
                                     MathGlyphs::rightCeilingBracket());
        break;
    case AngleBracket:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::rightAngleBracket(), true);
        break;
    case StraightBracket:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::straightBracket());
        break;
    case DoubleStraightBracket:
        x += renderSingleBracket(painter, x, halfHeight, MathGlyphs::doubleStraightBracket());
        break;
    default:
        break;
    }
    return x;
}

qreal QGenRenderer::renderDisplayWithParentheses(Display &dest, const Display &source, QPointF where)
{
    return renderDisplayWithBrackets(dest, source, BracketType::Parenthesis, BracketType::Parenthesis, where);
}

qreal QGenRenderer::renderDisplayWithPriority(Display &dest, const Display &source, int priority, QPointF where)
{
    QPointF penPoint(where);
    if (source.priority() < priority)
    {
        renderDisplayAndAdvance(dest, source, penPoint);
        return penPoint.x() - where.x();
    }
    return renderDisplayWithParentheses(dest, source, penPoint);
}

qreal QGenRenderer::renderDisplayWithSquareBrackets(Display &dest, const Display &source, QPointF where)
{
    return renderDisplayWithBrackets(dest, source, BracketType::SquareBracket, BracketType::SquareBracket, where);
}

qreal QGenRenderer::renderDisplayWithCurlyBrackets(Display &dest, const Display &source, QPointF where)
{
    return renderDisplayWithBrackets(dest, source, BracketType::CurlyBracket, BracketType::CurlyBracket, where);
}

void QGenRenderer::renderHorizontalLine(QPainter &painter, qreal x, qreal y, qreal length, qreal widthFactor)
{
    QRectF rect = textTightBoundingRect(MathGlyphs::horizontalLineExtension());
    qreal xOffset = rect.x() + 0.5, yOffset = rect.y() + rect.height() / 2, baseWidth = rect.width() - 1;
    int n = qRound(length / baseWidth);
    for (int i = 0; i < n; ++i)
        painter.drawText(QPointF(x + i * baseWidth - xOffset, y - yOffset), MathGlyphs::horizontalLineExtension());
    qreal f = length / baseWidth - n;
    if (f > 0)
    {
        painter.save();
        painter.translate(x + n * baseWidth, y);
        painter.scale(f, widthFactor);
        painter.drawText(QPointF(-xOffset, -yOffset), MathGlyphs::horizontalLineExtension());
        painter.restore();
    }
}

void QGenRenderer::renderHorizontalLine(Display &dest, QPointF where, qreal length, qreal widthFactor)
{
    QPainter painter(&dest);
    painter.setFont(getFont(fontSizeLevel()));
    renderHorizontalLine(painter, where.x(), where.y(), length, widthFactor);
}

qreal QGenRenderer::linePadding(int relativeSizeLevel)
{
    return textWidth(MathGlyphs::thinSpace(), relativeSizeLevel);
}

void QGenRenderer::renderDisplayWithAccent(Display &dest, const Display &source,
                                           AccentType accentType, QPointF where)
{
    renderDisplay(dest, source, QPointF(where.x() + source.leftBearing(), where.y()));
    QChar accent;
    bool stretchable = false;
    switch (accentType)
    {
    case Hat:
        accent = MathGlyphs::hatAccent();
        stretchable = true;
        break;
    case Check:
        accent = MathGlyphs::checkAccent();
        stretchable = true;
        break;
    case Tilde:
        accent = MathGlyphs::tildeAccent();
        stretchable = true;
        break;
    case Bar:
        accent = MathGlyphs::barAccent();
        stretchable = true;
        break;
    case Vec:
        accent = MathGlyphs::vecAccent();
        stretchable = true;
        break;
    case Acute:
        accent = MathGlyphs::acuteAccent();
        break;
    case Grave:
        accent = MathGlyphs::graveAccent();
        break;
    case Dot:
        accent = MathGlyphs::dotAccent();
        break;
    case DoubleDot:
        accent = MathGlyphs::doubleDotAccent();
        break;
    case TripleDot:
        accent = MathGlyphs::tripleDotAccent();
        break;
    }
    QRectF rect = textTightBoundingRect(accent);
    qreal xOffset = rect.x() + 0.5, yOffset = rect.y() + rect.height(), accentWidth = rect.width() - 1;
    QPainter painter(&dest);
    painter.setFont(getFont(fontSizeLevel()));
    painter.translate(where.x(), where.y() - source.ascent() - linePadding());
    if (stretchable)
    {
        qreal f = source.totalWidth() / accentWidth;
        painter.scale(f, 1.0);
        painter.drawText(QPointF(-xOffset, -yOffset), accent);
    }
    else
    {
        qreal w = source.totalWidth() - source.leftBearing();
        painter.drawText((w - accentWidth) / 2 - xOffset + source.leftBearing(), -yOffset, accent);
    }
}
