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

using namespace giac;

QString QGen::fontFamily = "FreeSerif";
QList<QFont> QGen::fonts = QList<QFont>();
QList<int> QGen::fontSizes = QList<int>();

QGen::QGen(const gen &e, GIAC_CONTEXT)
    : expr(new gen(e))
    , ct(contextptr)
    , owns(true) { }

QGen::QGen(gen *e, GIAC_CONTEXT)
    : expr(e)
    , ct(contextptr)
    , owns(false) { }

QGen::QGen(const QGen &e)
    : expr(new gen(e.expression()))
    , ct(e.contextPtr())
    , owns(true) { }

QGen::QGen(GIAC_CONTEXT)
    : expr(new gen(0))
    , ct(contextptr)
    , owns(true) { }

QGen::QGen(const QString &text, GIAC_CONTEXT)
    : expr(new gen(text.toStdString().data(), contextptr))
    , ct(contextptr)
    , owns(true) { }

QGen::QGen(int value, GIAC_CONTEXT)
    : expr(new gen(value))
    , ct(contextptr)
    , owns(true) { }

QGen::QGen(qreal value, GIAC_CONTEXT)
    : expr(new gen(value))
    , ct(contextptr)
    , owns(true) { }

QGen::~QGen()
{
    if (owns)
        delete expr;
}

QMap<QString, QGen::UserOperator> QGen::userOperators = QMap<QString, QGen::UserOperator>();

QGen QGen::makeSymb(const unary_function_ptr *p, const gen &args) const
{
    return QGen(symbolic(p, args), ct);
}

bool QGen::isNegativeConstant() const
{
    return  (isInteger() && integerValue() < 0) ||
            (isFloatingPointNumber() && doubleValue() < 0) ||
            (isRational() && expr->_FRACptr->num.val > 0 && expr->_FRACptr->den.val > 0);
}

bool QGen::isElementary() const {
    return  isUnaryFunction(at_exp) || isUnaryFunction(at_ln) || isUnaryFunction(at_log10) ||
            isUnaryFunction(at_sin) || isUnaryFunction(at_cos) || isUnaryFunction(at_tan) ||
            isUnaryFunction(at_sec) || isUnaryFunction(at_csc) || isUnaryFunction(at_cot) ||
            isUnaryFunction(at_asin) || isUnaryFunction(at_acos) || isUnaryFunction(at_atan) ||
            isUnaryFunction(at_acot) || isUnaryFunction(at_sinh) || isUnaryFunction(at_cosh) ||
            isUnaryFunction(at_tanh) || isUnaryFunction(at_asinh) || isUnaryFunction(at_acosh) ||
            isUnaryFunction(at_atanh) || isUnaryFunction(at_det) || isUnaryFunction(at_det_minor);
}

int QGen::operandCount() const
{
    return isSymbolic() && feuille().type == _VECT ? int(feuilleVector()->size()) : -1;
}

QGen QGen::firstOperand() const
{
    if (!isSymbolic() || operandCount() < 1)
        return undefined();
    return QGen(&(feuilleVector()->front()), ct);
}

QGen QGen::lastOperand() const
{
    if (!isSymbolic() || operandCount() < 1)
        return undefined();
    return QGen(&(feuilleVector()->back()), ct);
}

QGen QGen::secondOperand() const
{
    if (!isSymbolic() || operandCount() < 2)
        return undefined();
    return QGen(&(feuilleVector()->at(1)), ct);
}

QGen QGen::thirdOperand() const
{
    if (!isSymbolic() || operandCount() < 3)
        return undefined();
    return QGen(&(feuilleVector()->at(2)), ct);
}

QGen QGen::nthOperand(int n) const
{
    if (!isSymbolic() || operandCount() <= qAbs(n))
        return undefined();
    const vecteur &vect = *feuilleVector();
    return QGen(vect.at(n >= 0 ? n : int(vect.size()) + n), ct);
}

QGen QGen::valueForKey(const QGen &key) const
{
    return isMap() ? QGen(&(expr->_MAPptr->at(key.expression())), ct) : undefined();
}

bool QGen::nthMapEntry(int n, QGen &key, QGen &value) const
{
    if (!isMap() || mapEntriesCount() <= n)
        return false;
    gen_map::iterator it;
    for (int i = 0; i < n; ++i, ++it);
    key = QGen(it->first, ct);
    value = QGen(it->second, ct);
    return true;
}

bool QGen::isUnitApplicationOperator(bool &hasConstant) const
{
    if (!isUnitApplicationOperator())
        return false;
    vecteur &identifiers = *_lname(feuilleVector()->back(), ct)._VECTptr;
    const_iterateur it;
    for (it = identifiers.begin(); it != identifiers.end(); ++it)
    {
        if (QString(it->print(ct).data()).endsWith("_"))
        {
            hasConstant = true;
            break;
        }
    }
    return true;
}

bool QGen::isFunctionApplicationOperator(QString &functionName, QGen &arguments) const
{
    if (!isFunctionApplicationOperator())
        return false;
    vecteur &args = *feuilleVector();
    functionName = QString(args.front().print(ct).data());
    args.erase(args.begin());
    arguments = QGen(args, ct);
    return true;
}

bool QGen::isMappingOperator(QGen &variables, QGen &expression) const
{
    if (!isMappingOperator())
        return false;
    vecteur vect(*feuilleVector());
    expression = QGen(vect.back(), ct);
    vect.pop_back();
    variables = QGen(vect, ct);
    return true;
}

bool QGen::isIntervalOperator(QGen &lowerBound, QGen &upperBound) const
{
    if (!isIntervalOperator())
        return false;
    lowerBound = QGen(feuilleVector()->front(), ct);
    upperBound = QGen(feuilleVector()->back(), ct);
    return true;
}

bool QGen::isRationalExpression() const
{
    if (isReciprocalOperator())
        return true;
    if (isProductOperator())
    {
        const_iterateur it;
        vecteur &args = *feuilleVector();
        for (it = args.begin(); it != args.end(); ++it)
        {
            if (it->is_symb_of_sommet(at_inv))
                return true;
        }
    }
    return false;
}

bool QGen::isRationalExpression(QGen &numerator, QGen &denominator) const
{
    if (isReciprocalOperator())
    {
        numerator = QGen(1, ct);
        denominator = unaryFunctionArgument();
        return true;
    }
    if (!isProductOperator())
        return false;
    vecteur &factors = *feuilleVector();
    gen num, den;
    const_iterateur it;
    for (it = factors.begin(); it != factors.end(); ++it)
    {
        if (it->is_symb_of_sommet(at_inv))
            den = den * it->_SYMBptr->feuille;
        else
            num = *it * num;
    }
    numerator = QGen(num, ct);
    denominator = QGen(den, ct);
    return !is_one(den);
}

bool QGen::isDoubleLetterIdentifier() const
{
    QString text = toString();
    return isIdentifier() && text.length() == 2 && text.begin()->isLetter() && *text.begin() == *text.end();
}

bool QGen::isIdentifierWithName(const QString &name) const
{
    return expr->is_identificateur_with_name(name.toStdString().data());
}

bool QGen::isInequation() const
{
    return  isLessThanOperator() || isGreaterThanOperator() ||
            isLessThanOrEqualOperator() || isGreaterThanOrEqualOperator();
}

int QGen::isInequation(QGen &leftHandSide, QGen &rightHandSide) const
{
    if (!isInequation())
        return 0;
    leftHandSide = firstOperand();
    rightHandSide = secondOperand();
    if (isLessThanOperator())
        return InequalityType::LessThan;
    if (isLessThanOrEqualOperator())
        return InequalityType::LessThanOrEqualTo;
    if (isGreaterThanOperator())
        return InequalityType::GreaterThan;
    if (isGreaterThanOrEqualOperator())
        return InequalityType::GreaterThanOrEqualTo;
    return 0; // unreachable
}

int QGen::operatorType(int &priority) const
{
    if (isSumOperator() || isHadamardSumOperator()) {
        priority = AdditionPriority; return OperatorType::Associative; }
    if (isProductOperator() || isAmpersandProductOperator() || isHadamardProductOperator() || isComposeOperator()) {
        priority = MultiplicationPriority; return OperatorType::Associative; }
    if (isConjunctionOperator() || isDisjunctionOperator() || isExclusiveOrOperator()) {
        priority = LogicalPriority; return OperatorType::Associative; }
    if (isBitwiseAndOperator() || isBitwiseOrOperator() || isBitwiseXorOperator()) {
        priority = BitwisePriority; return OperatorType::Associative; }
    if (isUnionOperator() || isIntersectionOperator()) {
        priority = SetPriority; return OperatorType::Associative; }
    if (isMinusOperator() || isIncrementOperator() || isDecrementOperator()) {
        priority = AdditionPriority; return OperatorType::Unary; }
    if (isNegationOperator() || isRealPartOperator() || isImaginaryPartOperator() || isTraceOperator()) {
        priority = UnaryPriority; return OperatorType::Unary; }
    if (isComplexConjugateOperator() || isFactorialOperator() ||
            isTranspositionOperator() || isDerivativeOperator(true)) {
        priority = ExponentiationPriority; return OperatorType::Unary; }
    if (isReciprocalOperator()) {
        priority = DivisionPriority; return OperatorType::Unary; }
    if (isUnitApplicationOperator()) {
        priority = MultiplicationPriority; return OperatorType::Binary; }
    if (isPowerOperator() || isHadamardPowerOperator() || isFunctionalPowerOperator()) {
        priority = ExponentiationPriority; return OperatorType::Binary; }
    if (isHadamardDifferenceOperator()) {
        priority = DifferencePriority; return OperatorType::Binary; }
    if (isHadamardDivisionOperator()) {
        priority = MultiplicationPriority; return OperatorType::Binary; }
    if (isCrossProductOperator()) {
        priority = MultiplicationPriority; return OperatorType::Binary; }
    if (isSetDifferenceOperator()) {
        priority = SetPriority; return OperatorType::Binary; }
    if (isIntervalOperator()) {
        priority = IntervalPriority; return OperatorType::Binary; }
    if (isInequation()) {
        priority = InequationPriority; return OperatorType::Binary; }
    if (isElementOperator() || isEqualOperator() || isNotEqualOperator()) {
        priority = ComparisonPriority; return OperatorType::Binary; }
    if (isEquation()) {
        priority = EquationPriority; return OperatorType::Binary; }
    if (isFunctionApplicationOperator() || isAtOperator()) {
        priority = ApplicationPriority; return OperatorType::Binary; }
    if (isMappingOperator()) {
        priority = EquationPriority; return OperatorType::Binary; }
    if (isAssignmentOperator() || isArrayAssignmentOperator()) {
        priority = AssignmentPriority; return OperatorType::Binary; }
    if (isConditionalOperator() || isIfThenElseOperator()) {
        priority = ConditionalPriority; return OperatorType::Ternary; }
    return 0;
}

bool QGen::isOperator(int &type) const
{
    int priority, t;
    if ((t = operatorType(priority)) == 0)
        return false;
    type = t;
    return true;
}

bool QGen::isOperator() const
{
    int t;
    return isOperator(t);
}

bool QGen::isAssociativeOperator(vecteur &operands) const
{
    int t;
    if (!isOperator(t) || t != OperatorType::Associative)
        return false;
    operands = flattenOperands();
    return true;
}

bool QGen::isBinaryOperator(QGen &left, QGen &right) const
{
    int t;
    if (!isOperator(t) || t != OperatorType::Binary)
        return false;
    left = firstOperand();
    right = secondOperand();
    return true;
}

bool QGen::isUnaryOperator(QGen &operand) const
{
    int t;
    if (!isOperator(t) || t != OperatorType::Unary)
        return false;
    operand = unaryFunctionArgument();
    return true;
}

bool QGen::isTheSameOperatorAs(const QGen &g) const
{
    if (!isSymbolic() || !g.isSymbolic())
        return false;
    return sommet() == g.expression()._SYMBptr->sommet;
}

int QGen::operatorPriority() const
{
    int priority;
    if (operatorType(priority) == 0)
        return -1;
    return priority;
}

bool QGen::isDerivativeOperator(bool simple) const
{
    return isUnaryFunction(at_derive) && !simple == (feuille().type == _VECT);
}

bool QGen::isUserOperator(QString &name) const
{
    if (!isUserOperator())
        return false;
    name = QString(sommet().ptr()->s);
    return true;
}

bool QGen::isIndefiniteIntegral(QGen &expression, QGen &variable) const
{
    if (!isIntegral() || unaryFunctionArgument().length() != 2)
        return false;
    expression = firstOperand();
    variable = secondOperand();
    return true;
}

bool QGen::isDefiniteIntegral(QGen &expression, QGen &variable, QGen &lowerBound, QGen &upperBound) const
{
    if (!isIntegral() || operandCount() < 3)
        return false;
    expression = firstOperand();
    variable = secondOperand();
    if (operandCount() == 3)
    {
        QGen last = lastOperand();
        if (!last.isEquation() || !last.secondOperand().isIntervalOperator())
            return false;
        QGen interval = last.secondOperand();
        lowerBound = interval.firstOperand();
        upperBound = interval.secondOperand();
    }
    else if (operandCount() == 4)
    {
        lowerBound = thirdOperand();
        upperBound = lastOperand();
    }
    else return false;
    return true;
}

vecteur QGen::flattenOperands() const
{
    Q_ASSERT(isSymbolic());
    vecteur operands;
    gen argument = feuille();
    if (argument.type != _VECT)
        operands.push_back(argument);
    else
    {
        vecteur &argumentVector = *argument._VECTptr;
        const_iterateur it;
        for (int i = 0 ; i < int(argumentVector.size()); ++i) {
            QGen operand(&(argumentVector.at(i)), ct);
            if (isSymbolic() && operand.sommet() == this->sommet()) {
                vecteur subOperands = operand.flattenOperands();
                for (it = subOperands.begin(); it != subOperands.end(); ++it)
                    operands.push_back(*it);
            }
            else operands.push_back(operand.expression());
        }
    }
    return operands;
}

bool QGen::resizeVector(int n)
{
    if (!isVector())
        return false;
    expr->_VECTptr->resize(n >= 0 ? n : qMax(int(expr->_VECTptr->size()) + n, 0));
    return true;
}

QGen QGen::vectorPopFront()
{
    if (length() < 1)
        return undefined();
    QGen leading(expr->_VECTptr->front(), ct);
    expr->_VECTptr->erase(expr->_VECTptr->begin());
    return leading;
}

qreal QGen::approximateValue() const
{
    gen e = _evalf(*expr, ct);
    return e.type == _DOUBLE_ ? e.DOUBLE_val() : 0;
}

bool QGen::isComplex(QGen &realPart, QGen &imaginaryPart) const
{
    if (!isComplex())
        return false;
    realPart = QGen(expr->_CPLXptr, ct);
    imaginaryPart = QGen(expr->_CPLXptr + 1, ct);
    return true;
}

bool QGen::isFraction(QGen &numerator, QGen &denominator) const
{
    if (!isFraction())
        return false;
    numerator = QGen(&(expr->_FRACptr->num), ct);
    denominator = QGen(&(expr->_FRACptr->den), ct);
    return true;
}

bool QGen::isModular(QGen &value, QGen &modulus) const
{
    if (!isModular())
        return false;
    value = QGen(expr->_MODptr, ct);
    modulus = QGen(expr->_MODptr + 1, ct);
    return true;
}

QPicture QGen::toPicture(const QString &family, int size) const
{
}

QString QGen::toLaTeX() const
{
    QGen latex = _latex(*expr, ct);
    if (latex.isString())
        return latex.stringValue();
    return "";
}

QString QGen::toMathML() const
{
    QGen mathML = _mathml(*expr, ct);
    if (mathML.isString())
        return mathML.stringValue();
    return "";
}

gen QGen::seq(const gen &g1, const gen &g2)
{
    return makesequence(g1, g2);
}

gen QGen::seq(const gen &g1, const gen &g2, const gen &g3)
{
    return makesequence(g1, g2, g3);
}

gen QGen::seq(const gen &g1, const gen &g2, const gen &g3, const gen &g4)
{
    return makesequence(g1, g2, g3, g4);
}

gen QGen::vec(const gen &g)
{
    return makevecteur(g);
}

gen QGen::vec(const gen &g1, const gen &g2)
{
    return makevecteur(g1, g2);
}

gen QGen::vec(const gen &g1, const gen &g2, const gen &g3)
{
    return makevecteur(g1, g2, g3);
}

gen QGen::vec(const gen &g1, const gen &g2, const gen &g3, const gen &g4)
{
    return makevecteur(g1, g2, g3, g4);
}

QGen QGen::identifier(const QString &name, GIAC_CONTEXT)
{
    return QGen(identificateur(name.toStdString()), contextptr);
}

QGen QGen::string(const QString &text, GIAC_CONTEXT)
{
    return QGen(string2gen(text.toStdString()), contextptr);
}

QGen QGen::fraction(int numerator, int denominator, GIAC_CONTEXT)
{
    return QGen(giac::fraction(numerator, denominator), contextptr);
}

QGen QGen::binomial(const QGen &n, const QGen &k, GIAC_CONTEXT)
{
    return QGen(symbolic(at_binomial, seq(n.expression(), k.expression())), contextptr);
}

QGen QGen::plusInfinity(GIAC_CONTEXT)
{
    return QGen(symbolic(at_plus, _IDNT_infinity()), contextptr);
}

QGen QGen::minusInfinity(GIAC_CONTEXT)
{
    return QGen(symbolic(at_neg, _IDNT_infinity()), contextptr);
}

QGen QGen::undefined(GIAC_CONTEXT)
{
    return QGen(undef, contextptr);
}

QGen QGen::equation(const QGen &left, const QGen &right, GIAC_CONTEXT)
{
    return QGen(symbolic(at_equal, seq(left.expression(), right.expression())), contextptr);
}

bool QGen::defineBinaryOperator(const QString &name, OperatorPriority priority, const QString &symbol,
                                const QGen &realFunction, bool checkProperties, GIAC_CONTEXT)
{
    gen binary = gen(_BINARY_OPERATOR).change_subtype(_INT_MUPADOPERATOR);
    gen args = seq(string2gen(name.toStdString()), realFunction.expression(), binary);
    if (is_zero(user_operator(args, contextptr)))
        return false;
    UserOperator op;
    op.ct = contextptr;
    op.priority = priority;
    op.symbol = symbol;
    op.isRelation = false;
    if (checkProperties)
    {
        identificateur x(" x_tmp"), y(" y_tmp"), z(" z_tmp");
        gen xy = vec(x, y);
        gen &f = realFunction.expression();
        gen fxy = _apply(seq(f, vec(xy)), contextptr);
        gen d = _apply(seq(f, vec(makevecteur(z, fxy))), contextptr) - _apply(seq(f, vec(vec(xy, z))), contextptr);
        op.isAssociative = is_zero(_simplify(d, contextptr));
        d = fxy - _apply(seq(f, vec(vec(y, x))), contextptr);
        op.isCommutative = is_zero(_simplify(d, contextptr));
    }
    else
    {
        op.isAssociative = false;
        op.isCommutative = false;
    }
    return true;
}

bool QGen::defineBinaryRelation(const QString &name, OperatorPriority priority, const QString &symbol,
                                const QGen &booleanFunction, GIAC_CONTEXT)
{
    gen binary = gen(_BINARY_OPERATOR).change_subtype(_INT_MUPADOPERATOR);
    gen args = seq(string2gen(name.toStdString()), booleanFunction.expression(), binary);
    if (is_zero(user_operator(args, contextptr)))
        return false;
    UserOperator op;
    op.ct = contextptr;
    op.priority = priority;
    op.symbol = symbol;
    op.isRelation = true;
    op.isAssociative = false;
    op.isCommutative = false;
    return true;
}

bool QGen::findUserOperator(const QString &name, UserOperator &properties)
{
    QMap<QString, UserOperator>::const_iterator it = userOperators.constFind(name);
    if (it == userOperators.constEnd())
        return false;
    properties = *it;
    return true;
}

// RENDERING

void QGen::setRenderingFont(const QString &family, int basePointSize)
{
    fontFamily = family;
    for (int i = -2; i < 2; ++i)
        fontSizes << qRound(basePointSize * pow(2.0, (qreal)i / 2.0));
}

QString QGen::paddedText(const QString &text, MathPadding padding, bool padLeft, bool padRight)
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

QString QGen::quotedText(const QString &text)
{
    QString quoted(text);
    quoted.prepend(MathGlyphs::leftDoubleQuotationMark());
    quoted.append(MathGlyphs::rightDoubleQuotationMark());
    return quoted;
}

QString QGen::numberToSuperscriptText(int integer, bool parens)
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

QString QGen::numberToSubscriptText(int integer, bool parens)
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

QString QGen::identifierStringToUnicode(const QString &text, bool bold, bool italic)
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

const QFont &QGen::getFont(int fontSizeLevel)
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

void QGen::render(Display &dest, const QGen &g, int sizeLevel)
{
    fontSizeLevelStack.push(sizeLevel);
    QPointF origin(0.0, 0.0);
    QGen realPart, imaginaryPart;
    if (g.isString())
        renderText(dest, quotedText(g.stringValue()));
    else if (g.isRealConstant())
        renderRealNumber(dest, g, origin);
    else if (g.isComplex(realPart, imaginaryPart))
        renderComplexNumber(dest, realPart, imaginaryPart, origin);
    else if (g.isIdentifier())
        renderIdentifier(dest, g, origin);
    else if (g.isModular())
        renderModular(dest, g, origin);
    else if (g.isMap())
        renderMap(dest, g, origin);
    else if (g.isVector())
        renderVector(dest, g, origin);
    else if (g.isSymbolic())
        renderSymbolic(dest, g, origin);
    else
        renderText(dest, g.toString());
    fontSizeLevelStack.pop();
}

QGen::Display QGen::renderNormal(const QGen &g)
{
    Display display;
    render(display, g, fontSizeLevel());
    return display;
}

QGen::Display QGen::renderSmaller(const QGen &g)
{
    Display display;
    render(display, g, smallerFontSizeLevel());
    return display;
}

QGen::Display QGen::renderLarger(const QGen &g)
{
    Display display;
    render(display, g, largerFontSizeLevel());
    return display;
}

void QGen::renderDisplay(Display &dest, const Display &source, QPointF where)
{
    QPainter painter(&dest);
    painter.drawPicture(where, source);
}

void QGen::renderDisplayAndAdvance(Display &dest, const Display &source, QPointF &penPoint)
{
    renderDisplay(dest, source, penPoint);
    movePenPointX(penPoint, source.advance());
}

qreal QGen::renderText(Display &dest, const QString &text,
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

void QGen::renderTextAndAdvance(Display &dest, const QString &text, QPointF &penPoint)
{
    movePenPointX(penPoint, renderText(dest, text, 0, penPoint));
}

qreal QGen::textWidth(const QString &text, int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.width(text);
}

QRectF QGen::textTightBoundingRect(const QString &text, int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.tightBoundingRect(text);
}

qreal QGen::fontHeight(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.height();
}

qreal QGen::fontAscent(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.ascent();
}

qreal QGen::fontDescent(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.descent();
}

qreal QGen::fontXHeight(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.xHeight();
}

qreal QGen::fontMidLine(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return (fontMetrics.xHeight() / 2 + fontMetrics.strikeOutPos()) / 2;
}

qreal QGen::fontLeading(int relativeFontSizeLevel)
{
    QFontMetricsF fontMetrics(getFont(fontSizeLevel() + relativeFontSizeLevel));
    return fontMetrics.leading();
}

qreal QGen::lineWidth(int relativeFontSizeLevel)
{
    return textTightBoundingRect(MathGlyphs::horizontalLineExtension(), relativeFontSizeLevel).height();
}

void QGen::renderRealNumber(Display &dest, const QGen &g, QPointF where)
{
    QPointF penPoint(where);
    QGen gAbs(g), numerator, denominator;
    if (g.isNegativeConstant())
    {
        renderTextAndAdvance(dest, MathGlyphs::minus(), penPoint);
        dest.setPriority(QGen::MultiplicationPriority);
        gAbs = QGen(-g.expression(), ct);
    }
    if (gAbs.isFraction(numerator, denominator)) {
        renderFraction(dest, numerator, denominator, penPoint);
        dest.setPriority(QGen::DivisionPriority);
        dest.setGrouped(true);
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

void QGen::renderComplexNumber(Display &dest, const QGen &realPart, const QGen &imaginaryPart, QPointF where)
{
    QPointF penPoint(where);
    Display realPartDisplay = renderNormal(realPart);
    QGen imAbs = (imaginaryPart.isNegativeConstant() ? QGen(-imaginaryPart.expression(), ct) : imaginaryPart);
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

void QGen::renderIdentifier(Display &dest, const QGen &g, QPointF where)
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
            if ((italic = g.isPlaceholderIdentifier()))
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

void QGen::renderLeadingUnderscoreIdentifier(Display &dest, const QGen &g, QPointF where)
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

void QGen::renderModular(Display &dest, const QGen &g, QPointF where)
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

void QGen::renderUnary(Display &dest, const QGen &g, QPointF where)
{
    QPointF penPoint(where);
    QString prefix;
    QGen argument = g.unaryFunctionArgument();
    Display argumentDisplay;
    int priority = g.operatorPriority();
    if (priority == QGen::UnaryPriority || priority == QGen::AdditionPriority)
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

void QGen::renderBinary(Display &dest, const QGen &g, QPointF where)
{
    QPointF penPoint(where);
    int priority = g.operatorPriority(), rightVerticalPosition = 0;
    QGen left = g.firstOperand();
    QGen right = g.secondOperand();
    QChar op;
    bool hasConstant, withCircle;
    if (g.isPowerOperator() || (withCircle = g.isHadamardPowerOperator() || g.isFunctionalPowerOperator()))
        rightVerticalPosition = 1;
    else if (g.isEquation())
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
    else if (g.isFunctionApplicationOperator())
        op = MathGlyphs::functionApplicationSpace();
    else if (g.isAtOperator())
    {
        right = g.unaryFunctionArgument();
        left = right.vectorPopFront();
        rightVerticalPosition = -1;
    }
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
    switch (rightVerticalPosition)
    {
    case 0:
        movePenPointX(penPoint, renderDisplayWithPriority(dest, renderNormal(left), priority, penPoint));
        renderTextAndAdvance(dest, paddedText(op), penPoint);
        renderDisplayWithPriority(dest, renderNormal(right), priority, penPoint);
        break;
    case -1:
        renderSubscript(dest, left, right, priority, penPoint);
        break;
    case 1:
        renderSuperscript(dest, left, right, priority, penPoint, withCircle);
        break;
    }
    dest.setPriority(priority);
}

void QGen::renderAssociative(Display &dest, const QGen &g, const QVector<QGen> &operands, QPointF where)
{
    QString op;
    int priority = g.operatorPriority();
    if (g.isSumOperator() || g.isHadamardSumOperator())
        op = "+";
    else if (g.isProductOperator() || g.isAmpersandProductOperator())
        op = MathGlyphs::multiplicationDot();
    else if (g.isComposeOperator() || g.isHadamardProductOperator())
        op = MathGlyphs::ringOperator();
    else if (g.isUnionOperator())
        op = MathGlyphs::setUnion();
    else if (g.isIntersectionOperator())
        op = MathGlyphs::setIntersection();
    else if (g.isConjunctionOperator())
        op = MathGlyphs::logicalAnd();
    else if (g.isDisjunctionOperator())
        op = MathGlyphs::logicalOr();
    else if (g.isExclusiveOrOperator())
        op = MathGlyphs::logicalXor();
    else if (g.isBitwiseAndOperator())
        op = "&";
    else if (g.isBitwiseOrOperator())
        op = "|";
    else if (g.isBitwiseXorOperator())
        op = "XOR";
    else
        Q_ASSERT(false); // unreachable
    QVector<QGen>::const_iterator it;
    QList<Display> renderedOperands, renderedIdentifiers, renderedNumbers;
    for (it = operands.begin(); it != operands.end(); ++it)
    {
        const QGen &operand = *it;
        Display display;
        if (g.isSumOperator())
        {
            //display = renderNormal(operand.isPrecededByMinus() ? -operand : operand);
            if (operand.isPrecededByMinus())
            {
                display.requireMinusSign(true);
                renderedOperands.append(display);
            }
            else renderedOperands.prepend(display);
        }
        else if (g.isProductOperator())
        {
            display = renderNormal(operand);
            if (operand.isRealConstant())
                renderedNumbers.append(display);
            else if (operand.isIdentifier())
            {
                display.linkWithExpression(operand);
                renderedIdentifiers.append(display);
            }
            else
                renderedOperands.append(display);
        }
        else renderedOperands.append(renderNormal(operand));
    }
    QPointF penPoint(where);
    op = paddedText(op);

    dest.setPriority(priority);
}

void QGen::renderFraction(Display &dest, const QGen &numerator, const QGen &denominator, QPointF where)
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

void QGen::renderSuperscript(Display &dest, const QGen &base, const QGen &exponent,
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

void QGen::renderSubscript(Display &dest, const QGen &base, const QGen &subscript, int priority, QPointF where)
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

void QGen::renderDisplayWithRadical(Display &dest, const Display &source, QPointF where)
{
    QRectF rect = textTightBoundingRect(MathGlyphs::squareRoot());
    qreal rWidth = rect.width(), rHeight = rect.height(), rDescent = rect.y() + rHeight, rBearing = -rect.x();
    qreal radicandWidth = source.totalWidth() + linePadding();
    qreal radicandHeight = qMax(source.height() + linePadding(), rHeight);
    qreal fY = qMax(1.0, (radicandHeight + lineWidth() / 3) / rHeight), fX = 1.0;
    renderDisplay(dest, source, QPointF(where.x() + rWidth * fX + source.leftBearing() + linePadding(), where.y()));
    QPainter painter(&dest);
    painter.setFont(getFont(fontSizeLevel()));
    renderHorizontalLine(painter, rWidth * fX - lineWidth() / 3, radicandHeight - source.descent(), radicandWidth);
    painter.translate(where.x(), where.y() + source.descent());
    painter.scale(fX, fY);
    painter.drawText(QPointF(rBearing, -rDescent), MathGlyphs::squareRoot());
}

void QGen::renderMap(Display &dest, const QGen &g, QPointF where)
{
    Q_ASSERT(g.isMap());
    gen_map &map = *g.expression()._MAPptr;
    vecteur sequence;
    gen_map::const_iterator it;
    for (it = map.begin(); it != map.end(); ++it)
        sequence.push_back(symbolic(at_equal, it->first, it->second));
    renderVector(dest, QGen(sequence, ct).toSequenceVector(), where);
}

void QGen::renderVector(Display &dest, const QGen &g, QPointF where)
{

}

void QGen::renderSymbolic(Display &dest, const QGen &g, QPointF where)
{

}

void QGen::renderBracketExtensionFill(QPainter &painter, const QChar &extension,
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

qreal QGen::renderSingleBracket(QPainter &painter, qreal x, qreal halfHeight, QChar bracket, bool scaleX)
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

qreal QGen::renderSingleFillBracket(QPainter &painter, qreal x, qreal halfHeight,
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

qreal QGen::renderCurlyBracket(QPainter &painter, qreal x, qreal halfHeight, bool left)
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

qreal QGen::renderDisplayWithBrackets(Display &dest, const Display &source,
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

qreal QGen::renderDisplayWithParentheses(Display &dest, const Display &source, QPointF where)
{
    return renderDisplayWithBrackets(dest, source, BracketType::Parenthesis, BracketType::Parenthesis, where);
}

qreal QGen::renderDisplayWithPriority(Display &dest, const Display &source, int priority, QPointF where)
{
    QPointF penPoint(where);
    if (source.priority() < priority)
    {
        renderDisplayAndAdvance(dest, source, penPoint);
        return penPoint.x() - where.x();
    }
    return renderDisplayWithParentheses(dest, source, penPoint);
}

qreal QGen::renderDisplayWithSquareBrackets(Display &dest, const Display &source, QPointF where)
{
    return renderDisplayWithBrackets(dest, source, BracketType::SquareBracket, BracketType::SquareBracket, where);
}

qreal QGen::renderDisplayWithCurlyBrackets(Display &dest, const Display &source, QPointF where)
{
    return renderDisplayWithBrackets(dest, source, BracketType::CurlyBracket, BracketType::CurlyBracket, where);
}

void QGen::renderHorizontalLine(QPainter &painter, qreal x, qreal y, qreal length, qreal widthFactor)
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

void QGen::renderHorizontalLine(Display &dest, QPointF where, qreal length, qreal widthFactor)
{
    QPainter painter(&dest);
    painter.setFont(getFont(fontSizeLevel()));
    renderHorizontalLine(painter, where.x(), where.y(), length, widthFactor);
}

qreal QGen::linePadding(int relativeSizeLevel)
{
    return textWidth(MathGlyphs::thinSpace(), relativeSizeLevel);
}

void QGen::renderDisplayWithAccent(Display &dest, const Display &source,
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

void QGen::storeBoundingRect(gen &g, int x, int y, int width, int height)
{
    g._EQWptr->x = x;
    g._EQWptr->y = y;
    g._EQWptr->dx = width;
    g._EQWptr->dy = height;
    g._EQWptr->active = true;
}

void QGen::translateBoundingRect(gen &g, int dx, int dy)
{
    if (g._EQWptr->active)
    {
        g._EQWptr->x += dx;
        g._EQWptr->y += dy;
    }
}

QPicture QGen::render(int alignment)
{
    Display display;
    render(display, *this);
    qreal x = display.leftBearing(), y = 0.0;
    if ((alignment & AlignHCenter) != 0)
        x -= display.totalWidth() / 2.0;
    else if ((alignment & AlignRight) != 0)
        x -= display.totalWidth();
    if ((alignment & AlignTop) != 0)
        y += display.ascent();
    else if ((alignment & AlignBottom) != 0)
        y -= display.descent();
    else if ((alignment & AlignVCenter) != 0)
        y -= display.height() / 2.0 - display.descent();
    QPicture picture;
    QPainter painter(&picture);
    painter.drawPicture(QPointF(x, y), display);
    return picture;
}
