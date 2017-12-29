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
#include "qgenrenderer.h"

#define FONT_SIZE_SCALING_FACTOR 1.71526586621

QGenRenderer::QGenRenderer(const QString &family, int size)
{
    fontFamily = family;
    for (int i = -2; i < 2; ++i)
        fontSizes << qRound(size * pow(FONT_SIZE_SCALING_FACTOR, i));
    renderFontBold = false;
    renderFontItalic = false;
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
        raised.prepend(MathGlyphs::superscriptMinusSignSymbol());
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
        raised.prepend(MathGlyphs::subscriptMinusSignSymbol());
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

QFont QGenRenderer::makeFont(int fontSizeLevel)
{
    int level = 2 + (int)qMin(qMax(fontSizeLevel, -2), 1);
    QFont::Weight weight = renderFontBold ? QFont::Bold : QFont::Normal;
    return QFont(fontFamily, fontSizes.at(level), weight, renderFontItalic);
}

QGenRenderer::Display QGenRenderer::render(const QGen &g, int sizeLevel,
                                           BracketType leftBracketType, BracketType rightBracketType)
{
    Display display;
    fontSizeLevelStack.push(sizeLevel);
    if (g.isString())
        renderText(&display, quotedText(g.stringValue()));
    else if (g.isConstant())
        renderNumber(&display, g);
    else if (g.isIdentifier())
        renderIdentifier(&display, g);
    else if (g.isFunction())
        renderFunction(&display, g);
    else if (g.isModular())
        renderModular(&display, g);
    else if (g.isMap())
        renderMap(&display, g);
    else if (g.isVector())
        renderVector(&display, g.toVector());
    else if (g.isSymbolic())
        renderSymbolic(&display, g);
    else
        renderText(&display, g.toString());
    fontSizeLevelStack.pop();
    return display;
}

QPicture QGenRenderer::render(const QGen &g)
{
    return render(g, 0, None, None);
}

QGenRenderer::Display QGenRenderer::renderNormal(const QGen &g, bool parenthesize)
{
    BracketType leftBracketType = parenthesize ? LeftParenthesis : None;
    BracketType rightBracketType = parenthesize ? RightParenthesis : None;
    return render(g, fontSizeLevel(), leftBracketType, rightBracketType);
}

QGenRenderer::Display QGenRenderer::renderSmaller(const QGen &g, bool parenthesize)
{
    BracketType leftBracketType = parenthesize ? LeftParenthesis : None;
    BracketType rightBracketType = parenthesize ? RightParenthesis : None;
    return render(g, smallerFontSizeLevel(), leftBracketType, rightBracketType);
}

QGenRenderer::Display QGenRenderer::renderLarger(const QGen &g, bool parenthesize)
{
    BracketType leftBracketType = parenthesize ? LeftParenthesis : None;
    BracketType rightBracketType = parenthesize ? RightParenthesis : None;
    return render(g, largerFontSizeLevel(), leftBracketType, rightBracketType);
}

void QGenRenderer::renderDisplayed(QPaintDevice *device, const Display &displayed, QPointF where)
{
    QPainter painter(device);
    painter.drawPicture(where, displayed);
}

void QGenRenderer::renderDisplayedAndAdvance(QPaintDevice *device, const Display &displayed, QPointF &penPoint)
{
    renderDisplayed(device, displayed, penPoint);
    movePenPointX(penPoint, displayed.advance());
}

void QGenRenderer::renderLine(QPaintDevice *device, QPointF start, QPointF end)
{
    QPainter painter(device);
    painter.drawLine(start, end);
}

void QGenRenderer::renderHLine(QPaintDevice *device, QPointF start, qreal length)
{
    QPointF end(start.x() + length, start.y());
    renderLine(device, start, end);
}

qreal QGenRenderer::renderText(QPaintDevice *device, const QString &text,
                              int relativeFontSizeLevel, QPointF where, QRectF *boundingRect)
{
    QPainter painter(device);
    QFont font = makeFont(fontSizeLevel() + relativeFontSizeLevel);
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

void QGenRenderer::renderTextAndAdvance(QPaintDevice *device, const QString &text, QPointF &penPoint)
{
    movePenPointX(penPoint, renderText(device, text, 0, penPoint));
}

qreal QGenRenderer::textWidth(const QString &text, int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(makeFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.width(text);
}

QRectF QGenRenderer::textTightBoundingRect(const QString &text, int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(makeFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.tightBoundingRect(text);
}

qreal QGenRenderer::fontHeight(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(makeFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.height();
}

qreal QGenRenderer::fontAscent(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(makeFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.ascent();
}

qreal QGenRenderer::fontDescent(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(makeFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.descent();
}

qreal QGenRenderer::fontMidLine(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(makeFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.xHeight();
}

qreal QGenRenderer::fontCenter(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(makeFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.strikeOutPos();
}

qreal QGenRenderer::fontLeading(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(makeFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.leading();
}

void QGenRenderer::renderNumber(QPaintDevice *device, const QGen &g, QPointF where)
{
    QPointF penPoint(where);
    QGen gAbs = (g.isNegativeConstant() ? -g : g), numerator, denominator, realPart, imaginaryPart;
    if (g.isNegativeConstant())
        renderTextAndAdvance(device, MathGlyphs::minusSignSymbol(), penPoint);
    if (gAbs.isFraction(numerator, denominator))
        renderFraction(device, numerator, denominator, penPoint);
    else if (gAbs.isComplex(realPart, imaginaryPart))
    {
        Display realPartDisplay = renderNormal(realPart);
        QGen imAbs = (imaginaryPart.isNegativeConstant() ? -imaginaryPart : imaginaryPart);
        Display imAbsDisplay = renderNormal(imAbs);
        renderDisplayedAndAdvance(device, realPartDisplay, penPoint);
        QChar operatorChar = imaginaryPart.isNegativeConstant() ? MathGlyphs::minusSignSymbol() : '+';
        renderTextAndAdvance(device, paddedText(operatorChar), penPoint);
        renderDisplayedAndAdvance(device, imAbsDisplay, penPoint);
        QString imaginaryUnit("i");
        imaginaryUnit.prepend(MathGlyphs::invisibleTimesSpace());
        renderText(device, imaginaryUnit, 0, penPoint);
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
            text.append(paddedText(MathGlyphs::multiplicationSignSymbol()));
            QString exponentDigits = MathGlyphs::digitsToSuperscript(exponent);
            if (hasNegativeExponent)
                exponentDigits.prepend(MathGlyphs::superscriptMinusSignSymbol());
            text.append("10" + exponentDigits);
        }
        renderText(device, text, 0, penPoint);
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
    {
        bool italic = !(g.isPi() || g.isEulerNumber() || g.isEulerMascheroniConstant() || g.isImaginaryUnit());
        bool bold = false;
        QString text = g.toString();
        if (g.isAutoGeneratedIdentifier() || g.isDoubleLetterIdentifier())
        {
            bold = true;
            italic = g.isAutoGeneratedIdentifier();
            text = text.mid(1);
        }
        if (g.isEulerMascheroniConstant())
            text = "gamma";
        symbol = identifierStringToUnicode(text, bold, italic);
    }
    renderText(device, symbol, 0, where);
}

void QGenRenderer::renderPhysicalValue(QPaintDevice *device, const QGen &value, const QGen &unit, QPointF where)
{
    QString text = unit.toString(), opStr, subscript;
    bool italic = true, isUnit = false;
    if (text.endsWith("_"))
    {
        opStr = paddedText(MathGlyphs::multiplicationDotSymbol(), Thin);
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
            text = QString("q/m") + MathGlyphs::subscriptSmallLetterE();
        else if (text == "mp")
            text = QString("m") + MathGlyphs::subscriptSmallLetterP();
        else if (text == "mpme")
            text = QString("m") + MathGlyphs::subscriptSmallLetterP() +
                    QString("/m") + MathGlyphs::subscriptSmallLetterE();
        else if (text == "Rinfinity")
        {
            text = "R";
            subscript = MathGlyphs::infinitySymbol();
        }
        else if (QRegularExpression("^.(Sun|Earth)$").match(text).hasMatch())
        {
            subscript = text.mid(1);
            text = text.left(1);
        }
        if (text == "lambdac")
            text = "lambda";
        else if (text == "angl")
            text = QString("180") + MathGlyphs::degreeSymbol();
        else if (text == "twopi")
            text = QString("2") + QString(MathGlyphs::invisibleTimesSpace()) +
                    QString(MathGlyphs::smallLetterPi()) + MathGlyphs::thickSpace() + QString("rad");
        else if (text == "c3")
            text = "b";
        else if (text == "kq")
            text = "q/k";
        else if (text == "epsilon0q")
            text = MathGlyphs::smallLetterEpsilon() + MathGlyphs::digitsToSubscript("0") + "/q";
        else if (text == "qepsilon0")
            text = QString("q") + MathGlyphs::multiplicationDotSymbol() +
                    MathGlyphs::smallLetterEpsilon() + MathGlyphs::digitsToSubscript("0");
        else if (text == "epsilonsi")
        {
            text = MathGlyphs::smallLetterEpsilon();
            subscript = "r";
        }
        else if (text == "epsilonox")
            text = QString(MathGlyphs::smallLetterEpsilon()) + MathGlyphs::subscriptSmallLetterO() +
                    MathGlyphs::subscriptSmallLetterX();
        else if (QRegularExpression("^[a-z]+[0BN]$").match(text).hasMatch())
        {
            subscript = text.right(1);
            text.chop(1);
        }
    }
    else
    {
        isUnit = true;
        opStr = MathGlyphs::thickSpace();
        italic = false;
        text = text.mid(1);
        if (text == "deg")
            text = MathGlyphs::degreeSymbol();
        else if (text == "degreeF")
            text = MathGlyphs::degreeFahrenheitSymbol();
        else if (text == "Ohm")
            text = "Omega";
    }
    QPointF penPoint(where);
    if (isUnit || !value.isOne())
    {
        Display valueDisplay = renderNormal(value, value.isComplex());
        renderDisplayedAndAdvance(device, valueDisplay, penPoint);
    }
    QString symbol = identifierStringToUnicode(text, false, italic);
    if (isUnit || !value.isOne())
        symbol.prepend(opStr);
    renderTextAndAdvance(device, symbol, penPoint);
    setRenderingItalic(false);
    if (subscript.length() > 0)
    {
        movePenPointY(penPoint, fontMidLine(-1));
        renderText(device, subscript, -1, penPoint);
    }
}

void QGenRenderer::renderFunction(QPaintDevice *device, const QGen &g, int exponent, QPointF where)
{
    QPointF penPoint(where);
    QGen args = g.unaryFunctionArgument();
    bool noParens = g.isElementary() &&
            (args.isConstant() || args.isIdentifier() || args.isAbsoluteValue() || args.isRationalExpression());
    QString symbol = g.unaryFunctionName();
    QString digits("");
    if (symbol.length() == 1)
        symbol = MathGlyphs::letterToMath(*symbol.begin(), MathGlyphs::Serif, false, true);
    if (exponent > 0)
        digits = MathGlyphs::digitsToSuperscript(QString::number(exponent));
    symbol += digits;
    symbol.append(MathGlyphs::functionApplicationSpace());
    renderTextAndAdvance(device, symbol, penPoint);
    Display argsDisplay = renderNormal(args, !noParens);
    renderDisplayed(device, argsDisplay, penPoint);
}

void QGenRenderer::renderModular(QPaintDevice *device, const QGen &g, QPointF where)
{
    QGen value, modulus;
    Q_ASSERT(g.isModular(value, modulus));
    Display valueDisplay = renderNormal(value);
    Display modulusDisplay = renderNormal(modulus);
    QPointF penPoint(where);
    renderDisplayedAndAdvance(device, valueDisplay, penPoint);
    renderTextAndAdvance(device, " (mod ", penPoint);
    renderDisplayedAndAdvance(device, modulusDisplay, penPoint);
    renderText(device, ")", 0, penPoint);
}

void QGenRenderer::renderUnaryOperation(QPaintDevice *device, const QGen &g, QPointF where)
{
    QPointF penPoint(where);
    QString prefix;
    QGen arg = g.unaryFunctionArgument();
    Display argDisplay;
    if (g.isMinusOperator() || g.isNegationOperator() || g.isIncrementOperator() || g.isDecrementOperator())
    {
        if (g.isMinusOperator())
            prefix = MathGlyphs::minusSignSymbol();
        else if (g.isNegationOperator())
            prefix = MathGlyphs::notSignSymbol();
        else if (g.isIncrementOperator())
            prefix = "++";
        else if (g.isDecrementOperator())
            prefix = "--";
        renderTextAndAdvance(device, prefix, penPoint);
        argDisplay = renderNormal(arg);
        renderDisplayed(device, argDisplay, penPoint);
    }
    else if (g.isFactorialOperator())
    {
        argDisplay = renderNormal(arg, !arg.isIdentifier() && !arg.isInteger());
        renderDisplayedAndAdvance(device, argDisplay, penPoint);
        renderText(device, "!", 0, penPoint);
    }
    else if (g.isTranspositionOperator())
    {
        argDisplay = renderNormal(arg, !arg.isIdentifier());
        renderDisplayedAndAdvance(device, argDisplay, penPoint);
        Display operatorDisplay;
        renderText(&operatorDisplay, "T", -1, penPoint);
        movePenPointY(penPoint, -fontMidLine());
        renderDisplayed(device, operatorDisplay, penPoint);
    }
    else if (g.isDerivativeOperator(true))
    {
        int degree = 1;
        QGen arg = g.unaryFunctionArgument();
        while (arg.isDerivativeOperator(true))
        {
            ++degree;
            arg = g.unaryFunctionArgument();
        }
        argDisplay = renderNormal(arg, !arg.isIdentifier());
        renderDisplayedAndAdvance(device, argDisplay, penPoint);
        QString suffix;
        switch (degree)
        {
        case 1:
            suffix = MathGlyphs::primeSymbol();
            break;
        case 2:
            suffix = MathGlyphs::doublePrimeSymbol();
            break;
        case 3:
            suffix = MathGlyphs::triplePrimeSymbol();
            break;
        default:
            suffix = MathGlyphs::digitsToSuperscript(QString::number(degree));
            suffix.prepend(MathGlyphs::superscriptLeftParenthesis());
            suffix.append(MathGlyphs::superscriptRightParenthesis());
        }
        renderText(device, suffix, 0, penPoint);
    }
}

void QGenRenderer::renderBinaryOperation(QPaintDevice *device, const QGen &g, QPointF where)
{
    QPointF penPoint(where);
    QGen::Vector args = g.unaryFunctionArgument().toVector();
    if (g.isPowerOperator() || g.isHadamardPowerOperator() || g.isFunctionalPowerOperator())
    {
        QGen base = args.front(), exponent = args.back();
        renderPower(device, base, exponent, penPoint, g.isHadamardPowerOperator() || g.isFunctionalPowerOperator());
    }
    else
    {
        QGen leftOperand = args.front(), rightOperand = args.back();
        bool hasTopPriority = g.isEquation() || g.isNotEqualOperator() || g.isUnitApplicationOperator() ||
                g.isEqualOperator() || g.isInequation() || g.isAssignmentOperator();
        bool noRightParens = hasTopPriority || !rightOperand.isOperator();
        bool noLeftParens = hasTopPriority || !leftOperand.isOperator();
        Display leftOperandDisplay = renderNormal(leftOperand, !noLeftParens);
        Display rightOperandDisplay = renderNormal(rightOperand, !noRightParens);
        QString opStr;
        if (g.isEquation())
            opStr = paddedText("=");
        else if (g.isEqualOperator())
            opStr = paddedText(MathGlyphs::questionedEqualToSymbol());
        else if (g.isNotEqualOperator())
            opStr = paddedText(MathGlyphs::notEqualToSymbol());
        else if (g.isLessThanOperator())
            opStr = paddedText("<");
        else if (g.isGreaterThanOperator())
            opStr = paddedText(">");
        else if (g.isLessThanOrEqualOperator())
            opStr = paddedText(MathGlyphs::lessThanOrEqualToSymbol());
        else if (g.isGreaterThanOrEqualOperator())
            opStr = paddedText(MathGlyphs::greaterThanOrEqualToSymbol());
        else if (g.isAssignmentOperator())
            opStr = paddedText(MathGlyphs::equalToByDefinitionSymbol());
        else if (g.isCrossProductOperator())
            opStr = paddedText(MathGlyphs::multiplicationSignSymbol());
        else if (g.isSetDifferenceOperator())
            opStr = paddedText(MathGlyphs::setMinusSymbol());
        else if (g.isHadamardDivisionOperator())
            opStr = paddedText(MathGlyphs::circledDivisionSlashSymbol());
        else if (g.isHadamardDifferenceOperator())
            opStr = paddedText(MathGlyphs::minusSignSymbol());
        else
            opStr = paddedText("??");
        renderDisplayedAndAdvance(device, leftOperandDisplay, penPoint);
        renderTextAndAdvance(device, opStr, penPoint);
        renderDisplayed(device, rightOperandDisplay, penPoint);
    }
}

void QGenRenderer::renderAssociativeOperation(QPaintDevice *device, const QGen &g, QPointF where)
{
    QPointF penPoint(where);
    QString opStr;
    if (g.isProductOperator())
    {
        QGen numerator, denominator;
        if (g.isRationalExpression(numerator, denominator))
        {
            renderFraction(device, numerator, denominator, where);
            return;
        }
    }
    else if (g.isSumOperator() || g.isHadamardSumOperator())
        opStr = paddedText("+");
    else if (g.isUnionOperator())
        opStr = paddedText(MathGlyphs::unionSymbol());
    else if (g.isIntersectionOperator())
        opStr = paddedText(MathGlyphs::intersectionSymbol());
    else if (g.isConjunctionOperator())
        opStr = paddedText(MathGlyphs::logicalAndSymbol());
    else if (g.isDisjunctionOperator())
        opStr = paddedText(MathGlyphs::logicalOrSymbol());
    else if (g.isExclusiveOrOperator())
        opStr = paddedText(MathGlyphs::logicalXorSymbol());
    else if (g.isComposeOperator() || g.isHadamardProductOperator())
        opStr = paddedText(MathGlyphs::ringOperatorSymbol());
    else
        opStr = paddedText("??");
    QGen::Vector operands = g.associativeOperands();
    Display display;
    for (int i = 0; i < operands.length(); ++i)
    {
        QGen operand = operands.at(i);
        bool parenthesize = true;

        display = renderNormal(operand, parenthesize);
        renderDisplayedAndAdvance(device, display, penPoint);
        if (i < operands.length() - 1)
            renderTextAndAdvance(device, opStr, penPoint);
    }
}

void QGenRenderer::renderFraction(QPaintDevice *device, const QGen &numerator, const QGen &denominator, QPointF where)
{
    QPointF penPoint(where);
    Display numeratorDisplay = renderNormal(numerator);
    Display denominatorDisplay = renderNormal(denominator);
    qreal width = qMax(numeratorDisplay.width(), denominatorDisplay.width());
    qreal padding = fontLeading() + 0.5;
    movePenPointY(penPoint, -fontCenter() + 0.5);
    renderHLine(device, penPoint, width);
    QPointF numeratorPenPoint(penPoint), denominatorPenPoint(penPoint);
    movePenPointX(numeratorPenPoint, numeratorDisplay.leftBearing() + (width - numeratorDisplay.width()) / 2.0);
    movePenPointY(numeratorPenPoint, -(padding + numeratorDisplay.descent()));
    renderDisplayed(device, numeratorDisplay, penPoint);
    movePenPointX(denominatorPenPoint, denominatorDisplay.leftBearing() + (width - denominatorDisplay.width()) / 2.0);
    movePenPointY(denominatorPenPoint, padding + denominatorDisplay.ascent());
    renderDisplayed(device, denominatorDisplay, penPoint);
}

void QGenRenderer::renderPower(QPaintDevice *device, const QGen &base, const QGen &exponent, QPointF where, bool circ)
{
    QPointF penPoint(where);
    Display baseDisplay, exponentDisplay;
    baseDisplay = renderNormal(base, !base.isOperator());
    exponentDisplay = renderSmaller(exponent);
    renderDisplayedAndAdvance(device, baseDisplay, penPoint);
    qreal verticalOffset = qMax(0.0, baseDisplay.ascent() - fontAscent());
    movePenPointXY(penPoint, exponentDisplay.leftBearing(), -(fontMidLine() + verticalOffset));
    if (circ)
        movePenPointX(penPoint, renderText(device, MathGlyphs::ringOperatorSymbol(), -1, penPoint));
    renderDisplayed(device, exponentDisplay, penPoint);
}

void QGenRenderer::renderRadical(QPaintDevice *device, const QGen &argument, int degree, QPointF where)
{
    Q_ASSERT(degree > 0);
    QPointF penPoint(where);
    Display argumentDisplay = renderNormal(argument);
    bool isTall = argumentDisplay.ascent() > fontAscent();
    if (degree == 2 || (!isTall && degree < 5))
    {
        if (isTall)
        {
            QPointF lowerPoint(penPoint);
            renderTextAndAdvance(device, MathGlyphs::radicalSymbolBottom(), penPoint);
            renderDisplayed(device, argumentDisplay, penPoint);
            QRectF rect = textTightBoundingRect(MathGlyphs::radicalSymbolBottom());
            QPointF topRightCorner(rect.width() + rect.x(), rect.y() - rect.height());
            lowerPoint += topRightCorner;
            QPointF upperPoint(lowerPoint.x(), argumentDisplay.ascent() + topRightCorner.y());
            QPointF rightPoint(upperPoint.x() + argumentDisplay.advance(), upperPoint.y());
            renderLine(device, lowerPoint, upperPoint);
            renderLine(device, upperPoint, rightPoint);
        }
        else
        {
            QChar rootSymbol;
            switch (degree)
            {
            case 2:
                rootSymbol = MathGlyphs::squareRootSymbol();
                break;
            case 3:
                rootSymbol = MathGlyphs::cubeRootSymbol();
                break;
            case 4:
                rootSymbol = MathGlyphs::fourthRootSymbol();
                break;
            }
            renderTextAndAdvance(device, rootSymbol, penPoint);
            renderDisplayed(device, argumentDisplay, penPoint);
            movePenPointY(penPoint, -fontAscent());
            renderHLine(device, penPoint, argumentDisplay.advance());
        }
    }
    else renderPower(device, argument, QGen::fraction(1, degree, argument.context), penPoint);
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
