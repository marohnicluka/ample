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

QGen::~QGen()
{
}

QMap<QString, QGen::UserDefinedOperator> QGen::userDefinedOperators = QMap<QString, QGen::UserDefinedOperator>();

QGen QGen::makeSymb(const giac::unary_function_ptr *p, const giac::gen &args) const
{
    return QGen(giac::symbolic(p, args), context);
}

QString QGen::toString() const
{
    std::stringstream ss;
    ss << (giac::gen)*this;
    return QString(ss.str().data());
}

bool QGen::isNegativeConstant() const
{
    return  (isInteger() && integerValue() < 0) ||
            (isDouble() && doubleValue() < 0) ||
            (isRational() && approximateValue() > 0);
}

bool QGen::isElementary() const {
    return  isUnaryFunction(giac::at_exp) || isUnaryFunction(giac::at_ln) || isUnaryFunction(giac::at_log10) ||
            isUnaryFunction(giac::at_sin) || isUnaryFunction(giac::at_cos) || isUnaryFunction(giac::at_tan) ||
            isUnaryFunction(giac::at_sec) || isUnaryFunction(giac::at_csc) || isUnaryFunction(giac::at_cot) ||
            isUnaryFunction(giac::at_asin) || isUnaryFunction(giac::at_acos) || isUnaryFunction(giac::at_atan) ||
            isUnaryFunction(giac::at_acot) || isUnaryFunction(giac::at_sinh) || isUnaryFunction(giac::at_cosh) ||
            isUnaryFunction(giac::at_tanh) || isUnaryFunction(giac::at_asinh) || isUnaryFunction(giac::at_acosh) ||
            isUnaryFunction(giac::at_atanh) || isUnaryFunction(giac::at_det) || isUnaryFunction(giac::at_det_minor);
}

int QGen::argumentCount() const
{
    return isSymbolic() && _SYMBptr->feuille.type == giac::_VECT ? int(_SYMBptr->feuille._VECTptr->size()) : -1;
}

QGen QGen::firstArgument() const
{
    if (!isSymbolic() || argumentCount() < 1)
        return undefined();
    return QGen(_SYMBptr->feuille._VECTptr->front(), context);
}

QGen QGen::secondArgument() const
{
    if (!isSymbolic() || argumentCount() < 2)
        return undefined();
    return QGen(_SYMBptr->feuille._VECTptr->at(1), context);
}

QGen QGen::thirdArgument() const
{
    if (!isSymbolic() || argumentCount() < 3)
        return undefined();
    return QGen(_SYMBptr->feuille._VECTptr->at(2), context);
}

bool QGen::isUnitApplicationOperator(bool &hasConstant) const
{
    if (!isUnitApplicationOperator())
        return false;
    Vector identifiers = QGen(giac::_lname(secondArgument(), context)).toVector();
    Vector::const_iterator it;
    for (it = identifiers.begin(); it != identifiers.end(); ++it)
    {
        if (it->toString().endsWith("_"))
        {
            hasConstant = true;
            break;
        }
    }
    return true;
}

bool QGen::isFunctionApplicationOperator(QString &functionName, Vector &arguments) const
{
    if (!isFunctionApplicationOperator())
        return false;
    Vector args = unaryFunctionArgument().toVector();
    functionName = args.front().toString();
    args.pop_front();
    arguments = args;
    return true;
}

bool QGen::isMapsToOperator(Vector &variables, QGen &expression) const
{
    if (!isMapsToOperator())
        return false;
    variables = firstArgument().toVector();
    expression = secondArgument();
    return true;
}

bool QGen::isIntervalOperator(QGen &lowerBound, QGen &upperBound) const
{
    if (!isIntervalOperator())
        return false;
    lowerBound = firstArgument();
    upperBound = secondArgument();
    return true;
}

bool QGen::isRationalExpression() const
{
    if (isReciprocalOperator())
        return true;
    if (isProductOperator())
    {
        giac::const_iterateur it;
        giac::vecteur &args(*_SYMBptr->feuille._VECTptr);
        for (it = args.begin(); it != args.end(); ++it)
        {
            if (it->is_symb_of_sommet(giac::at_inv))
                return true;
        }
    }
    return false;
}

bool QGen::isRationalExpression(QGen &numerator, QGen &denominator) const
{
    if (isReciprocalOperator())
    {
        numerator = QGen(1, context);
        denominator = unaryFunctionArgument();
        return true;
    }
    if (!isProductOperator())
        return false;
    QGen::Vector factors = unaryFunctionArgument().toVector();
    QGen::Vector::const_iterator it;
    numerator = QGen(1, context);
    denominator = QGen(1, context);
    for (it = factors.begin(); it != factors.end(); ++it)
    {
        if (it->isReciprocalOperator())
            denominator = denominator * it->unaryFunctionArgument();
        else
            numerator = *it * numerator;
    }
    return !denominator.isOne();
}

bool QGen::isDoubleLetterIdentifier() const
{
    QString text = toString();
    return isIdentifier() && text.length() == 2 && text.begin()->isLetter() && *text.begin() == *text.end();
}

bool QGen::isInequation() const
{
    return  isLessThanOperator() || isGreaterThanOperator() ||
            isLessThanOrEqualOperator() || isGreaterThanOrEqualOperator();
}

int QGen::operatorType(int &priority) const
{
    if (isSumOperator() || isHadamardSumOperator()) {
        priority = 6; return OperatorType::Associative; }
    if (isProductOperator() || isAmpersandProductOperator() || isHadamardProductOperator() || isComposeOperator()) {
        priority = 5; return OperatorType::Associative; }
    if (isConjunctionOperator() || isDisjunctionOperator() || isExclusiveOrOperator()) {
        priority = 11; return OperatorType::Associative; }
    if (isBitwiseAndOperator() || isBitwiseOrOperator() || isBitwiseXorOperator()) {
        priority = 10; return OperatorType::Associative; }
    if (isUnionOperator() || isIntersectionOperator()) {
        priority = 7; return OperatorType::Associative; }
    if (isMinusOperator() || isIncrementOperator() || isDecrementOperator() || isNegationOperator()) {
        priority = 4; return OperatorType::Unary; }
    if (isComplexConjugateOperator() || isFactorialOperator()) {
        priority = 2; return OperatorType::Unary; }
    if (isReciprocalOperator()) {
        priority = 3; return OperatorType::Unary; }
    if (isTranspositionOperator()) {
        priority = 2; return OperatorType::Unary; }
    if (isUnitApplicationOperator()) {
        priority = 5; return OperatorType::Binary; }
    if (isPowerOperator() || isHadamardPowerOperator() || isFunctionalPowerOperator()) {
        priority = 2; return OperatorType::Binary; }
    if (isHadamardDifferenceOperator()) {
        priority = 6; return OperatorType::Binary; }
    if (isHadamardDivisionOperator()) {
        priority = 5; return OperatorType::Binary; }
    if (isCrossProductOperator()) {
        priority = 5; return OperatorType::Binary; }
    if (isSetDifferenceOperator()) {
        priority = 7; return OperatorType::Binary; }
    if (isIntervalOperator()) {
        priority = 13; return OperatorType::Binary; }
    if (isInequation()) {
        priority = 8; return OperatorType::Binary; }
    if (isElementOperator() || isEqualOperator() || isNotEqualOperator()) {
        priority = 9; return OperatorType::Binary; }
    if (isEquation()) {
        priority = 14; return OperatorType::Binary; }
    if (isFunctionApplicationOperator()) {
        priority = 1; return OperatorType::Binary; }
    if (isMapsToOperator()) {
        priority = 14; return OperatorType::Binary; }
    if (isWhenOperator() || isIfThenElseOperator()) {
        priority = 12; return OperatorType::Ternary; }
    if (isAssignmentOperator() || isArrayAssignmentOperator()) {
        priority = 15; return OperatorType::Other; }
    if (isAtOperator()) {
        priority = 1; return OperatorType::Other; }
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

bool QGen::isAssociativeOperator(Vector &arguments) const
{
    int t;
    if (!isOperator(t) || t != OperatorType::Associative)
        return false;
    arguments = flattenOperands();
    return true;
}

bool QGen::isBinaryOperator(QGen &firstOperand, QGen &secondOperand) const
{
    int t;
    if (!isOperator(t) || t != OperatorType::Binary)
        return false;
    firstOperand = firstArgument();
    secondOperand = secondArgument();
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

int QGen::operatorPriority() const
{
    int priority;
    if (operatorType(priority) == 0)
        return -1;
    return priority;
}

bool QGen::isDerivativeOperator(bool simple) const
{
    return isUnaryFunction(giac::at_derive) && !simple == unaryFunctionArgument().isVector();
}

bool QGen::isUserOperator(QString &name) const
{
    if (!isUserOperator())
        return false;
    name = QString(_SYMBptr->sommet.ptr()->s);
    return true;
}

bool QGen::isMonolithic() const
{
    if (!isSymbolic())
        return isIdentifier() || isInteger() || isDouble() || (isVector() && !isSequenceVector()) || isMatrix();
}

bool QGen::isIndefiniteIntegral(QGen &expression, QGen &variable) const
{
    if (!isIntegral() || unaryFunctionArgument().length() != 2)
        return false;
    expression = firstArgument();
    variable = secondArgument();
    return true;
}

bool QGen::isDefiniteIntegral(QGen &expression, QGen &variable, QGen &lowerBound, QGen &upperBound) const
{
    if (!isIntegral() || unaryFunctionArgument().length() < 3)
        return false;
    Vector arguments = unaryFunctionArgument().toVector();
    expression = arguments[0];
    variable = arguments[1];
    if (arguments.length() == 3)
    {
        if (!arguments.back().isEquation() || !arguments.back().secondArgument().isIntervalOperator())
            return false;
        QGen interval = arguments.back().secondArgument();
        lowerBound = interval.firstArgument();
        upperBound = interval.secondArgument();
    }
    else
    {
        lowerBound = arguments[2];
        upperBound = arguments[3];
    }
    return true;
}

QGen::Vector QGen::flattenOperands() const
{
    Q_ASSERT(isSymbolic());
    Vector operands;
    QGen argument = unaryFunctionArgument();
    if (!argument.isVector())
        operands.push_back(argument);
    else
    {
        Vector argumentVector = argument.toVector();
        Vector::iterator it;
        for (int i = 0 ; i < argumentVector.length(); ++i) {
            QGen operand = argumentVector.at(i);
            if (operand.isUnaryFunction(unaryFunctionPointer())) {
                Vector subOperands = operand.flattenOperands();
                for (it = subOperands.begin(); it != subOperands.end(); ++it)
                    operands.push_back(*it);
            }
            else operands.push_back(operand);
        }
    }
    return operands;
}

QGen::Vector QGen::toVector() const
{
    Vector vector;
    if (isVector())
    {
        giac::const_iterateur it;
        for (it = _VECTptr->begin(); it != _VECTptr->end(); ++it)
            vector.append(QGen(*it, context));
    }
    return vector;
}

qreal QGen::approximateValue() const
{
    QGen e(giac::_evalf(*this, context), context);
    return e.isDouble() ? e.doubleValue() : 0;
}

bool QGen::isComplex(QGen &realPart, QGen &imaginaryPart) const
{
    if (!isComplex())
        return false;
    realPart = *_CPLXptr;
    imaginaryPart = *(_CPLXptr + 1);
    return true;
}

bool QGen::isFraction(QGen &numerator, QGen &denominator) const
{
    if (!isFraction())
        return false;
    numerator = _FRACptr->num;
    denominator = _FRACptr->den;
    return true;
}

bool QGen::isModular(QGen &value, QGen &modulus) const
{
    if (!isModular())
        return false;
    value = *_MODptr;
    modulus = *(_MODptr + 1);
    return true;
}

QPicture QGen::toPicture(const QString &family, int size) const
{
}

QString QGen::toLaTeX() const
{
    QGen latex = giac::_latex(*this, context);
    if (latex.isString())
        return latex.stringValue();
    return "";
}

QString QGen::toMathML() const
{
    QGen mathML = giac::_mathml(*this, context);
    if (mathML.isString())
        return mathML.stringValue();
    return "";
}

giac::gen QGen::seq(const giac::gen &g1, const giac::gen &g2)
{
    return giac::makesequence(g1, g2);
}

giac::gen QGen::seq(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3)
{
    return giac::makesequence(g1, g2, g3);
}

giac::gen QGen::seq(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3, const giac::gen &g4)
{
    return giac::makesequence(g1, g2, g3, g4);
}

giac::gen QGen::vec(const giac::gen &g)
{
    return giac::makevecteur(g);
}

giac::gen QGen::vec(const giac::gen &g1, const giac::gen &g2)
{
    return giac::makevecteur(g1, g2);
}

giac::gen QGen::vec(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3)
{
    return giac::makevecteur(g1, g2, g3);
}

giac::gen QGen::vec(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3, const giac::gen &g4)
{
    return giac::makevecteur(g1, g2, g3, g4);
}

QGen QGen::operator ()(const Vector &arguments) const
{
    if (!isFunction())
        return undefined();
    giac::vecteur args;
    Vector::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); ++it)
        args.push_back(*it);
    return QGen(giac::_apply(seq(*this, vec(args)), context)._VECTptr->front(), context);
}

QGen QGen::operator ()(const QGen &g) const
{
    if (isVector() && g.isInteger() && g.integerValue() < length())
        return QGen(_VECTptr->at(g.integerValue()), context);
    Vector arguments;
    arguments.append(g);
    return this->operator ()(arguments);
}

QGen QGen::operator ()(const QGen &g1, const QGen &g2) const
{
    if (isMatrix() && g1.isInteger() && g2.isInteger() &&
            g1.integerValue() < rows() && g2.integerValue() < columns())
        return QGen(giac::_at(seq(*this, vec(g1, g2)), context), context);
    Vector arguments;
    arguments.append(g1);
    arguments.append(g2);
    return this->operator ()(arguments);
}

QGen QGen::operator ()(const QGen &g1, const QGen &g2, const QGen &g3) const
{
    Vector arguments;
    arguments.append(g1);
    arguments.append(g2);
    arguments.append(g3);
    return this->operator ()(arguments);
}

QGen QGen::operator ()(const QGen &g1, const QGen &g2, const QGen &g3, const QGen &g4) const
{
    Vector arguments;
    arguments.append(g1);
    arguments.append(g2);
    arguments.append(g3);
    arguments.append(g4);
    return this->operator ()(arguments);
}

QGen QGen::identifier(const QString &name, const giac::context *ctx)
{
    return QGen(giac::identificateur(name.toStdString()), ctx);
}

QGen QGen::string(const QString &text, const giac::context *ctx)
{
    return QGen(giac::string2gen(text.toStdString()), ctx);
}

QGen QGen::fraction(int numerator, int denominator, const giac::context *ctx)
{
    return QGen(giac::fraction(numerator, denominator), ctx);
}

QGen QGen::binomial(const QGen &n, const QGen &k, const giac::context *ctx)
{
    return QGen(giac::symbolic(giac::at_binomial, giac::makesequence(n, k)), ctx);
}

QGen QGen::plusInfinity(const giac::context *ctx)
{
    return QGen(giac::symbolic(giac::at_plus, giac::_IDNT_infinity()), ctx);
}

QGen QGen::minusInfinity(const giac::context *ctx)
{
    return QGen(giac::symbolic(giac::at_neg, giac::_IDNT_infinity()), ctx);
}

bool QGen::defineBinaryOperator(const QString &name, const QString &symbol,
                                const QGen &realFunction, bool checkProperties, const giac::context *ctx)
{
    giac::gen binary = giac::gen(giac::_BINARY_OPERATOR).change_subtype(giac::_INT_MUPADOPERATOR);
    giac::gen args = seq(giac::string2gen(name.toStdString()), realFunction, binary);
    if (giac::is_zero(giac::user_operator(args, ctx)))
        return false;
    UserDefinedOperator op;
    op.symbol = symbol;
    op.isRelation = false;
    if (checkProperties)
    {
        QGen x = QGen::identifier(" x_tmp"), y = QGen::identifier(" y_tmp"), z = QGen::identifier(" z_tmp");
        QGen d = realFunction(z,realFunction(x,y)) - realFunction(realFunction(x,y),z);
        op.isAssociative = d.simplify().isZero();
        d = realFunction(x,y) - realFunction(y,x);
        op.isCommutative = d.simplify().isZero();
    }
    else
    {
        op.isAssociative = false;
        op.isCommutative = false;
    }
    return true;
}

bool QGen::defineBinaryRelation(const QString &name, const QString &symbol,
                                const QGen &booleanFunction, const giac::context *ctx)
{
    giac::gen binary = giac::gen(giac::_BINARY_OPERATOR).change_subtype(giac::_INT_MUPADOPERATOR);
    giac::gen args = seq(giac::string2gen(name.toStdString()), booleanFunction, binary);
    if (giac::is_zero(giac::user_operator(args, ctx)))
        return false;
    UserDefinedOperator op;
    op.symbol = symbol;
    op.isRelation = true;
    op.isAssociative = false;
    op.isCommutative = false;
    return true;
}

bool QGen::findUserDefinedOperator(const QString &name, UserDefinedOperator &properties)
{
    QMap<QString, UserDefinedOperator>::const_iterator it = userDefinedOperators.constFind(name);
    if (it == userDefinedOperators.constEnd())
        return false;
    properties = *it;
    return true;
}
