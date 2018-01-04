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

#ifndef QGEN_H
#define QGEN_H

#define _GLIBCXX_USE_CXX11_ABI 0

#include <QPicture>
#include <QStringList>
#include <QMap>
#include <giac/config.h>
#include <giac/giac.h>
#include "mathglyphs.h"

typedef giac::unary_function_ptr GiacFunctionPointer;

class QGen : public giac::gen
{
public:
    typedef QList<QGen> Vector;

    enum OperatorPriority
    {
        ApplicationPriority     = 1,
        ExponentiationPriority  = 2,
        DivisionPriority        = 3,
        UnaryPriority           = 4,
        ModularPriority         = 5,
        MultiplicationPriority  = 6,
        DifferencePriority      = 7,
        AdditionPriority        = 8,
        SetPriority             = 9,
        InequationPriority      = 10,
        ComparisonPriority      = 11,
        BitwisePriority         = 12,
        LogicalPriority         = 13,
        ConditionalPriority     = 14,
        IntervalPriority        = 15,
        EquationPriority        = 16,
        AssignmentPriority      = 17,
        CommaPriority           = 18
    };

    struct UserOperator
    {
        const giac::context *context;
        OperatorPriority priority;
        QString symbol;
        bool isRelation;
        bool isAssociative;
        bool isCommutative;
    };

private:
    static QMap<QString, UserOperator> userOperators;

    static giac::gen seq(const giac::gen &g1, const giac::gen &g2);
    static giac::gen seq(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3);
    static giac::gen seq(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3, const giac::gen &g4);

    static giac::gen vec(const giac::gen &g);
    static giac::gen vec(const giac::gen &g1, const giac::gen &g2);
    static giac::gen vec(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3);
    static giac::gen vec(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3, const giac::gen &g4);

    QGen undefined() const { return QGen(giac::undef, context); }
    QGen makeSymb(const giac::unary_function_ptr *p, const giac::gen &args) const;
    Vector flattenOperands() const;

public:
    enum InequalityType { LessThan = -2, GreaterThan = 2, LessThanOrEqualTo = -1, GreaterThanOrEqualTo = 1 };
    enum OperatorType { Associative = 1, Unary = 2, Binary = 3, Ternary = 4 };

    const giac::context *context;

    QGen(const giac::context *ctx = giac::context0) : giac::gen(0), context(ctx) { }
    QGen(const QGen &e) : giac::gen(e), context(e.context) { }
    QGen(const giac::gen &e, const giac::context *ctx = giac::context0) : giac::gen(e), context(ctx) { }
    QGen(const QString &text, const giac::context *ctx = giac::context0)
        : giac::gen(text.toStdString().data(), ctx), context(ctx) { }
    QGen(int value, const giac::context *ctx = giac::context0) : giac::gen(value), context(ctx) { }
    QGen(qreal value, const giac::context *ctx = giac::context0) : giac::gen(value), context(ctx) { }

    ~QGen();

    static QGen identifier(const QString &name, const giac::context *ctx = giac::context0);
    static QGen string(const QString &text, const giac::context *ctx = giac::context0);
    static QGen fraction(int numerator, int denominator, const giac::context *ctx = giac::context0);
    static QGen binomial(const QGen &n, const QGen &k, const giac::context *ctx = giac::context0);
    static QGen plusInfinity(const giac::context *ctx = giac::context0);
    static QGen minusInfinity(const giac::context *ctx = giac::context0);
    static bool defineBinaryOperator(const QString &name, OperatorPriority priority, const QString &symbol,
                                     const QGen &realFunction, bool checkProperties,
                                     const giac::context *ctx = giac::context0);
    static bool defineBinaryRelation(const QString &name, OperatorPriority priority, const QString &symbol,
                                     const QGen &booleanFunction, const giac::context *ctx = giac::context0);
    static bool findUserOperator(const QString &name, UserOperator &properties);

    QString toString() const { return QString(print(context).data()); }
    QString toLaTeX() const;
    QString toMathML() const;
    QPicture toPicture(const QString &family, int size) const;

    bool isString() const { return type == giac::_STRNG; }
    bool isUnaryFunction(const GiacFunctionPointer *f) const { return is_symb_of_sommet(f); }
    GiacFunctionPointer *unaryFunctionPointer() const { return isSymbolic() ? &_SYMBptr->sommet : NULL; }
    QString unaryFunctionName() const { return isSymbolic() ? unaryFunctionPointer()->ptr()->print(context) : ""; }
    QGen unaryFunctionArgument() const { return isSymbolic() ? QGen(_SYMBptr->feuille, context) : undefined(); }

    bool isIdentifier() const { return type == giac::_IDNT; }
    bool isPlaceholderIdentifier() const { return isIdentifier() && toString().startsWith("` "); }
    bool isDoubleLetterIdentifier() const;
    bool isInfinityIdentifier() const { return isIdentifier() && *_IDNTptr == giac::_IDNT_infinity(); }
    bool isUndefIdentifier() const { return isIdentifier() && *_IDNTptr == giac::_IDNT_undef(); }
    bool isIdentifierWithName(const QString &name) const { return is_identificateur_with_name(name.toStdString().data()); }
    bool isVector() const { return type == giac::_VECT; }
    bool isVectorOfLength(int length) const { return is_vector_of_size(length); }
    bool isOrdinaryVector() const { return isVector() && subtype == 0; }
    bool isSequenceVector() const { return isVector() && subtype == giac::_SEQ__VECT; }
    bool isSetVector() const { return isVector() && subtype == giac::_SET__VECT; }
    bool isListVector() const { return isVector() && subtype == giac::_LIST__VECT; }
    bool isPolynomVector() const { return isVector() && subtype == giac::_POLY1__VECT; }
    bool isMatrixVector() const { return isVector() && subtype == giac::_MATRIX__VECT; }
    bool isAssumeVector() const { return isVector() && subtype == giac::_ASSUME__VECT; }
    bool isSymbolic() const { return type == giac::_SYMB; }
    bool isMap() const { return type == giac::_MAP; }
    bool isFraction() const { return type == giac::_FRAC; }
    bool isFraction(QGen &numerator, QGen &denominator) const;
    bool isConstant() const { return is_constant() || isFraction(); }
    bool isMultiPrecisionFloat() const { return type == giac::_REAL; }
    bool isFloat() const { return type == giac::_FLOAT_; }
    bool isDouble() const { return type == giac::_DOUBLE_; }
    bool isComplex() const { return type == giac::_CPLX; }
    bool isComplex(QGen &realPart, QGen &imaginaryPart) const;
    bool isInteger() const { return is_integer(); }
    bool isBoolean() const { return type == giac::_INT_ && subtype == giac::_INT_BOOLEAN; }
    bool isRational() const { return isFraction() && _FRACptr->num.is_integer() && _FRACptr->den.is_integer(); }
    bool isExact() const { return giac::is_exact(*this); }
    bool isModular() const { return type == giac::_MOD; }
    bool isModular(QGen &value, QGen &modulus) const;
    bool isMatrix() const { return isMatrixVector() || giac::ckmatrix(*this); }
    bool isSparsePolynomial() const { return type == giac::_SPOL1; }
    bool isFunction() const { return type == giac::_FUNC || isUnaryFunction(giac::at_program); }
    bool isAlgebraicExtension() const { return type == giac::_EXT; }
    bool isUserDefined() const { return type == giac::_USER; }
    bool isUndefined() const { return giac::is_undef(*this); }
    bool isInfinite() const { return giac::is_inf(*this); }
    bool isZero() const { return giac::is_zero(*this, context); }
    bool isExactlyZero() const { return giac::is_exactly_zero(*this); }
    bool isVectorOfZeros() const { return isVector() ? giac::is_zero__VECT(*this->_VECTptr, context) : false; }
    bool isOne() const { return giac::is_one(*this); }
    bool isMinusOne() const { return giac::is_minus_one(*this); }
    bool isEulerMascheroniConstant() const { return isIdentifier() && toString() == "euler_gamma"; }
    bool isEulerNumber() const { return isIdentifier() && toString() == "e"; }
    bool isPi() const { return isIdentifier() && *_IDNTptr == giac::_IDNT_pi(); }
    bool isImaginaryUnit() const { return isIdentifier() && toString() == "i"; }
    bool isNonnegative() const { return giac::is_positive(*this, context); }
    bool isNegative() const { return !isNonnegative(); }
    bool isPositive() const { return giac::is_strictly_positive(*this, context); }
    bool isRationalExpression() const;
    bool isRationalExpression(QGen &numerator, QGen &denominator) const;
    bool isWellDefined() const { return !giac::has_inf_or_undef(*this); }
    bool isElementary() const;

    bool isNegativeConstant() const;
    bool elementaryNeedsArgumentBrackets() const;

    bool containsApproximateValue() const { return is_approx(); }
    bool containsErrorMessage(QString &message) const;
    bool containsImaginaryUnit() const { return giac::has_i(*this); }

    bool isPolynomialRoot() const { return isUnaryFunction(giac::at_rootof); }

    // operators
    bool isAssignmentOperator() const { return isUnaryFunction(giac::at_sto); }
    bool isArrayAssignmentOperator() const { return isUnaryFunction(giac::at_array_sto); }
    bool isAtOperator() const { return isUnaryFunction(giac::at_at); }
    bool isSumOperator() const { return isUnaryFunction(giac::at_plus); }
    bool isIncrementOperator() const { return isUnaryFunction(giac::at_increment); }
    bool isDecrementOperator() const { return isUnaryFunction(giac::at_decrement); }
    bool isHadamardSumOperator() const { return isUnaryFunction(giac::at_pointplus); }
    bool isHadamardDifferenceOperator() const { return isUnaryFunction(giac::at_pointminus); }
    bool isProductOperator() const { return isUnaryFunction(giac::at_prod); }
    bool isAmpersandProductOperator() const { return isUnaryFunction(giac::at_ampersand_times); }
    bool isHadamardProductOperator() const { return isUnaryFunction(giac::at_pointprod); }
    bool isFactorialOperator() const { return isUnaryFunction(giac::at_factorial); }
    bool isCrossProductOperator() const { return isUnaryFunction(giac::at_cross); }
    bool isComposeOperator() const { return isUnaryFunction(giac::at_compose); }
    bool isPowerOperator() const { return isUnaryFunction(giac::at_pow); }
    bool isHadamardPowerOperator() const { return isUnaryFunction(giac::at_pointpow); }
    bool isFunctionalPowerOperator() const { return isUnaryFunction(giac::at_composepow); }
    bool isReciprocalOperator() const { return isUnaryFunction(giac::at_inv); }
    bool isHadamardDivisionOperator() const { return isUnaryFunction(giac::at_pointdivision); }
    bool isConjunctionOperator() const { return isUnaryFunction(giac::at_and); }
    bool isDisjunctionOperator() const { return isUnaryFunction(giac::at_ou); }
    bool isExclusiveOrOperator() const { return isUnaryFunction(giac::at_xor); }
    bool isNegationOperator() const { return isUnaryFunction(giac::at_not); }
    bool isBitwiseAndOperator() const { return isUnaryFunction(giac::at_bitand); }
    bool isBitwiseOrOperator() const { return isUnaryFunction(giac::at_bitor); }
    bool isBitwiseXorOperator() const { return isUnaryFunction(giac::at_bitxor); }
    bool isComplexConjugateOperator() const { return isUnaryFunction(giac::at_conj); }
    bool isMinusOperator() const { return isUnaryFunction(giac::at_neg); }
    bool isTranspositionOperator() const { return isUnaryFunction(giac::at_tran); }
    bool isTraceOperator() const { return isUnaryFunction(giac::at_trace); }
    bool isImaginaryPartOperator() const { return isUnaryFunction(giac::at_im); }
    bool isRealPartOperator() const { return isUnaryFunction(giac::at_re); }
    bool isUnionOperator() const { return isUnaryFunction(giac::at_union); }
    bool isIntersectionOperator() const { return isUnaryFunction(giac::at_intersect); }
    bool isSetDifferenceOperator() const { return isUnaryFunction(giac::at_minus); }
    bool isElementOperator() const { return isUnaryFunction(giac::at_contains); }
    bool isEqualOperator() const { return isUnaryFunction(giac::at_same); }
    bool isNotEqualOperator() const { return isUnaryFunction(giac::at_different); }
    bool isLessThanOperator() const { return isUnaryFunction(giac::at_inferieur_strict); }
    bool isGreaterThanOperator() const { return isUnaryFunction(giac::at_superieur_strict); }
    bool isLessThanOrEqualOperator() const { return isUnaryFunction(giac::at_inferieur_egal); }
    bool isGreaterThanOrEqualOperator() const { return isUnaryFunction(giac::at_superieur_egal); }
    bool isConditionalOperator() const { return isUnaryFunction(giac::at_when); }
    bool isIfThenElseOperator() const { return isUnaryFunction(giac::at_ifte); }
    bool isUnitApplicationOperator() const { return isUnaryFunction(giac::at_unit); }
    bool isUnitApplicationOperator(bool &hasConstant) const;
    bool isFunctionApplicationOperator() const { return isUnaryFunction(giac::at_of); }
    bool isFunctionApplicationOperator(QString &functionName, Vector &arguments) const;
    bool isMappingOperator() const { return isUnaryFunction(giac::at_program); }
    bool isMappingOperator(Vector &variables, QGen &expression) const;
    bool isIntervalOperator() const { return isUnaryFunction(giac::at_interval); }
    bool isIntervalOperator(QGen &lowerBound, QGen &upperBound) const;
    bool isDerivativeOperator(bool simple) const;
    bool isSequenceOperator() const { return isUnaryFunction(giac::at_dollar); }
    bool isUserOperator() const { return dynamic_cast<const giac::unary_function_user*>(_SYMBptr->sommet.ptr()) != 0; }
    bool isUserOperator(QString &name) const;

    int operatorType(int &priority) const;
    int operatorPriority() const;
    bool isOperator(int &type) const;
    bool isOperator() const;
    bool isAssociativeOperator(Vector &arguments) const;
    bool isBinaryOperator(QGen &left, QGen &right) const;
    bool isUnaryOperator(QGen &operand) const;

    bool isBinomialCoefficient() const { return isUnaryFunction(giac::at_binomial); }
    bool isBinomialCoefficient(int &n, int &k) const;
    bool isAbsoluteValue() const { return isUnaryFunction(giac::at_abs); }
    bool isEquation() const { return isUnaryFunction(giac::at_equal); }
    bool isEquation(QGen &leftHandSide, QGen &rightHandSide) const;
    bool isInequation() const;
    int isInequation(QGen &leftHandSide, QGen &rightHandSide) const;
    bool isIntegral() const { return isUnaryFunction(giac::at_integrate); }
    bool isIndefiniteIntegral(QGen &expression, QGen &variable) const;
    bool isDefiniteIntegral(QGen &expression, QGen &variable, QGen &lowerBound, QGen &upperBound) const;

    int integerValue() const { return isInteger() ? this->val : 0; }
    qreal doubleValue() const { return isDouble() ? this->DOUBLE_val() : 0; }
    qreal approximateValue() const;
    QString stringValue() const { return isString() ? this->_STRNGptr->data() : ""; }

    int operandCount() const;
    QGen firstOperand() const;
    QGen lastOperand() const;
    QGen secondOperand() const;
    QGen thirdOperand() const;
    QGen nthOperand(int n) const;

    QGen evaluate() const { return QGen(giac::_eval(*this, context), context); }
    QGen simplify() const { return QGen(giac::_simplify(*this, context), context); }

    int length() const { return isVector() ? _VECTptr->size() : -1; }
    int rows() const { return isMatrix() ? giac::_dim(*this, context)._VECTptr->front().val : -1; }
    int columns() const { return isMatrix() ? giac::_dim(*this, context)._VECTptr->back().val : -1; }
    Vector toVector() const;
    QGen &toSequenceVector() { if (isVector()) this->operator =(change_subtype(giac::_SEQ__VECT)); return *this; }
    QGen &toSetVector() { if (isVector()) this->operator =(change_subtype(giac::_SET__VECT)); return *this; }
    QGen &toListVector() { if (isVector()) this->operator =(change_subtype(giac::_LIST__VECT)); return *this; }
    QGen &toPolynomVector() { if (isVector()) this->operator =(change_subtype(giac::_POLY__VECT)); return *this; }
    void resizeVector(int n);

    QGen& operator =(const QGen &other) { ((giac::gen*)this)->operator =(other); context = other.context; return *this; }
    QGen& operator +=(const QGen &other) { ((giac::gen*)this)->operator =(giac::operator +=(*this, other)); return *this; }
    QGen& operator -=(const QGen &other) { ((giac::gen*)this)->operator =(giac::operator -=(*this, other)); return *this; }

    QGen operator ()(const QGen &g) const;
    QGen operator ()(const QGen &g1, const QGen &g2) const;
    QGen operator ()(const QGen &g1, const QGen &g2, const QGen &g3) const;
    QGen operator ()(const QGen &g1, const QGen &g2, const QGen &g3, const QGen &g4) const;
    QGen operator ()(const Vector &arguments) const;
};

inline QGen operator +(const QGen &a, const QGen &b) { return QGen(giac::operator +(a, b), a.context); }
inline QGen operator -(const QGen &a) { return QGen(giac::operator -(a), a.context); }
inline QGen operator -(const QGen &a, const QGen &b) { return QGen(giac::operator -(a, b), a.context); }
inline QGen operator *(const QGen &a, const QGen &b) { return QGen(giac::operator *(a, b), a.context); }
inline QGen operator /(const QGen &a, const QGen &b) { return QGen(giac::operator /(a, b), a.context); }
inline QGen operator &&(const QGen &a, const QGen &b) { return QGen(giac::operator &&(a, b), a.context); }
inline QGen operator ||(const QGen &a, const QGen &b) { return QGen(giac::operator ||(a, b), a.context); }

#endif // QGEN_H
