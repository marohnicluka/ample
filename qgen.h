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
#include <sstream>
#include <giac/config.h>
#include <giac/giac.h>
#include "mathglyphs.h"

typedef giac::unary_function_ptr GiacFunctionPointer;

class QGen : public giac::gen
{

public:
    typedef QList<QGen> Vector;
    enum LimitDirectionType
    {
        FromLeft = -1,
        FromRight = 1
    };

private:

    giac::gen qString2gen(const QString &str) const { return giac::gen(str.toStdString().c_str(), contextPtr); }

    giac::gen seq(const giac::gen &g1, const giac::gen &g2) const;
    giac::gen seq(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3) const;
    giac::gen seq(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3, const giac::gen &g4) const;

    QGen undefined() const { return QGen(giac::undef, contextPtr); }
    QGen makeSymb(const giac::unary_function_ptr *p, const giac::gen &args) const;
    Vector flattenOperands() const;

public:
    const giac::context *contextPtr;

    static QGen identifier(const QString &name, const giac::context *ctx);
    static QGen string(const QString &text, const giac::context *ctx);
    static QGen fraction(int numerator, int denominator, const giac::context *ctx);
    static QGen binomial(int n, int k, const giac::context *ctx);
    static QGen plusInfinity(const giac::context *ctx);
    static QGen minusInfinity(const giac::context *ctx);

    QGen(const giac::context *ctx = 0)
        : giac::gen(0)
        , contextPtr(ctx) { }

    QGen(const QGen &e)
        : giac::gen(e)
        , contextPtr(e.contextPtr) { }

    QGen(const giac::gen &e, const giac::context *ctx = 0)
        : giac::gen(e)
        , contextPtr(ctx) { }

    QGen(const QString &text, const giac::context *ctx = 0)
        : giac::gen(text.toStdString().c_str(), ctx)
        , contextPtr(ctx) { }

    QGen(int value, const giac::context *ctx = 0)
        : giac::gen(value)
        , contextPtr(ctx) { }

    QGen(qreal value, const giac::context *ctx = 0)
        : giac::gen(value)
        , contextPtr(ctx) { }

    ~QGen();

    QString toString() const;
    QString toLaTeX() const;
    QString toMathML() const;
    QPicture toPicture(const QString &family, int size) const;

    bool isString() const { return type == giac::_STRNG; }
    bool isIdentifier() const { return type == giac::_IDNT; }
    bool isIdentifierWithName(const QString &name) const { return is_identificateur_with_name(name.toStdString().c_str()); }
    bool isVector() const { return type == giac::_VECT; }
    bool isVectorOfLength(int length) const { return is_vector_of_size(length); }
    bool isSequenceVector() const { return isVector() && subtype == giac::_SEQ__VECT; }
    bool isSetVector() const { return isVector() && subtype == giac::_SET__VECT; }
    bool isAssumeVector() const { return isVector() && subtype == giac::_ASSUME__VECT; }
    bool isPolynomVector() const { return isVector() && subtype == giac::_POLY1__VECT; }
    bool isMatrixVector() const { return isVector() && subtype == giac::_MATRIX__VECT; }
    bool isListVector() const { return isVector() && subtype == giac::_LIST__VECT; }
    bool isSymbolic() const { return type == giac::_SYMB; }
    bool isMap() const { return type == giac::_MAP; }
    bool isFraction() const { return type == giac::_FRAC; }
    bool isConstant() const { return is_constant() || isFraction(); }
    bool isMultiPrecisionFloat() const { return type == giac::_REAL; }
    bool isFloat() const { return type == giac::_FLOAT_; }
    bool isDouble() const { return type == giac::_DOUBLE_; }
    bool isComplex() const { return type == giac::_CPLX; }
    bool isInteger() const { return is_integer(); }
    bool isExact() const { return giac::is_exact(*this); }
    bool isApproximate() const { return is_approx(); }
    bool isModular() const { return type == giac::_MOD; }
    bool isMatrix() const { return isMatrixVector() || giac::ckmatrix(*this); }
    bool isSparsePolynomial() const { return type == giac::_SPOL1; }
    bool isFunction() const { return type == giac::_FUNC; }
    bool isElementaryFunction() const;
    bool isAlgebraicExtension() const { return type == giac::_EXT; }
    bool isUserDefined() const { return type == giac::_USER; }
    bool isUndefined() const { return giac::is_undef(*this); }
    bool isInfinite() const { return giac::is_inf(*this); }
    bool isZero() const { return giac::is_zero(*this, contextPtr); }
    bool isExactlyZero() const { return giac::is_exactly_zero(*this); }
    bool isVectorOfZeros() const { return isVector() ? giac::is_zero__VECT(*this->_VECTptr, contextPtr) : false; }
    bool isOne() const { return giac::is_one(*this); }
    bool isMinusOne() const { return giac::is_minus_one(*this); }
    bool isNonnegative() const { return giac::is_positive(*this, contextPtr); }
    bool isNegative() const { return !isNonnegative(); }
    bool isPositive() const { return giac::is_strictly_positive(*this, contextPtr); }
    bool isRational() const;
    bool hasMinusSign() const;

    bool isSommet(const GiacFunctionPointer *f) const { return is_symb_of_sommet(f); }
    GiacFunctionPointer *getSommet() const { return isSymbolic() ? &_SYMBptr->sommet : NULL; }
    QString sommetName() const { return isSymbolic() ? getSommet()->ptr()->print(contextPtr) : ""; }
    QGen getFeuille() const { return isSymbolic() ? QGen(_SYMBptr->feuille, contextPtr) : undefined(); }

    bool isPolynomialRoot() const { return isSommet(giac::at_rootof); }

    bool isSummation() const { return isSommet(giac::at_plus); }
    bool isHadamardSummation() const { return isSommet(giac::at_pointplus); }
    bool isHadamardDifference() const { return isSommet(giac::at_pointminus); }
    bool isProduct() const { return isSommet(giac::at_prod); }
    bool isHadamardProduct() const { return isSommet(giac::at_pointprod); }
    bool isPower() const { return isSommet(giac::at_pow); }
    bool isHadamardPower() const { return isSommet(giac::at_pointpow); }
    bool isReciprocal() const { return isSommet(giac::at_inv); }
    bool isHadamardDivision() const { return isSommet(giac::at_pointdivision); }
    bool isConjunction() const { return isSommet(giac::at_and); }
    bool isDisjunction() const { return isSommet(giac::at_ou); }
    bool isExclusiveOr() const { return isSommet(giac::at_xor); }
    bool isNegation() const { return isSommet(giac::at_not); }
    bool isMinus() const { return isSommet(giac::at_neg); }
    bool isEqual() const { return isSommet(giac::at_same); }
    bool isDifferent() const { return isSommet(giac::at_different); }
    bool isLessThan() const { return isSommet(giac::at_inferieur_strict); }
    bool isGreaterThan() const { return isSommet(giac::at_superieur_strict); }
    bool isLessThanOrEqualTo() const { return isSommet(giac::at_inferieur_egal); }
    bool isGreaterThanOrEqualTo() const { return isSommet(giac::at_superieur_egal); }
    bool isIncrement() const { return isSommet(giac::at_increment); }
    bool isDecrement() const { return isSommet(giac::at_decrement); }
    bool isUnion() const { return isSommet(giac::at_union); }
    bool isIntersection() const { return isSommet(giac::at_intersect); }
    bool isSetDifference() const { return isSommet(giac::at_minus); }
    bool isFactorial() const { return isSommet(giac::at_factorial); }
    bool isDerivative() const { return isSommet(giac::at_derive); }
    bool isIntegral() const { return isSommet(giac::at_integrate); }
    bool isIfThenElse() const { return isSommet(giac::at_when); }
    bool isDefinition() const { return isSommet(giac::at_sto); }
    bool isComposition() const { return isSommet(giac::at_compose); }
    bool isCompositionPower() const { return isSommet(giac::at_composepow); }
    bool isEquation() const { return isSommet(giac::at_equal); }
    bool isCrossProduct() const { return isSommet(giac::at_cross); }
    bool isInterval() const { return isSommet(giac::at_interval); }
    bool isBinomial() const { return isSommet(giac::at_binomial); }
    bool isAbsoluteValue() const { return isSommet(giac::at_abs); }

    bool isUnaryOperator() const;
    bool isBinaryOperator() const;
    bool isAssociativeOperator() const;
    bool isInequalityOperator() const;

    int integerValue() const { return isInteger() ? this->val : 0; }
    qreal doubleValue() const { return isApproximate() ? this->DOUBLE_val() : 0; }
    qreal approximateValue() const;
    QString stringValue() const { return isString() ? this->_STRNGptr->data() : ""; }
    int fractionNumerator() const { return isFraction() ? _FRACptr->num.val : 0; }
    int fractionDenominator() const { return isFraction() ? _FRACptr->den.val : 0; }

    bool getModularComponents(QGen &value, QGen &modulus) const;

    QGen evaluate() { return QGen(giac::_eval(*this, contextPtr), contextPtr); }

    Vector toVector() const;
    int operandCount() const { return isSymbolic() && getFeuille().isVector() ? int(getFeuille().toVector().size()) : -1; }

    QGen operator +(const QGen &other) { return QGen((giac::gen)*this + other, contextPtr); }
    QGen operator +(int other) { return QGen((giac::gen)*this + other, contextPtr); }
    QGen operator +(qreal other) { return QGen((giac::gen)*this + other, contextPtr); }

    QGen operator -(const QGen &other) { return QGen((giac::gen)*this - other, contextPtr); }
    QGen operator -(int other) { return QGen((giac::gen)*this - other, contextPtr); }
    QGen operator -(qreal other) { return QGen((giac::gen)*this - other, contextPtr); }

    QGen operator *(const QGen &other) { return QGen((giac::gen)*this * other, contextPtr); }
    QGen operator *(int other) { return QGen((giac::gen)*this * (giac::gen)other, contextPtr); }
    QGen operator *(qreal other) { return QGen((giac::gen)*this * (giac::gen)other, contextPtr); }

    QGen operator /(const QGen &other) { return QGen((giac::gen)*this / other, contextPtr); }
    QGen operator /(int other) { return QGen((giac::gen)*this / other, contextPtr); }
    QGen operator /(qreal other) { return QGen((giac::gen)*this / other, contextPtr); }

    QGen operator &&(const QGen &other) { return QGen(giac::symbolic(giac::at_and, seq(*this, other)), contextPtr); }
    QGen operator ||(const QGen &other) { return QGen(giac::symbolic(giac::at_ou, seq(*this, other)), contextPtr); }
    QGen operator !() { return QGen(giac::symbolic(giac::at_not, *this), contextPtr); }

    QGen& operator =(const QGen &other) { giac::gen::operator =(other); contextPtr = other.contextPtr; return *this; }
    QGen& operator =(int number) { giac::gen::operator =(number); return *this; }
    QGen& operator =(qreal number) { giac::gen::operator =(number); return *this; }
    QGen& operator =(const QString &s) { giac::gen::operator =(qString2gen(s)); return *this; }

    QGen& operator +=(const QGen &other) { giac::gen::operator =((giac::gen)*this + other); return *this; }
    QGen& operator +=(int number) { giac::gen::operator =((giac::gen)*this + number); return *this; }
    QGen& operator +=(qreal number) { giac::gen::operator =((giac::gen)*this + number); return *this; }

    QGen& operator -=(const QGen &other) { giac::gen::operator =((giac::gen)*this - other); return *this; }
    QGen& operator -=(int number) { giac::gen::operator =((giac::gen)*this - number); return *this; }
    QGen& operator -=(qreal number) { giac::gen::operator =((giac::gen)*this - number); return *this; }

    QGen& operator *=(const QGen &other) { giac::gen::operator =((giac::gen)*this * other); return *this; }
    QGen& operator *=(int number) { giac::gen::operator =((giac::gen)*this * (giac::gen)number); return *this; }
    QGen& operator *=(qreal number) { giac::gen::operator =((giac::gen)*this * (giac::gen)number); return *this; }

    QGen& operator /=(const QGen &other) { giac::gen::operator =((giac::gen)*this / other); return *this; }
    QGen& operator /=(int number) { giac::gen::operator =((giac::gen)*this / number); return *this; }
    QGen& operator /=(qreal number) { giac::gen::operator =((giac::gen)*this / number); return *this; }

    bool operator <(const QGen &other) { return giac::is_strictly_greater(other, *this, contextPtr); }
    bool operator >(const QGen &other) { return giac::is_strictly_greater(*this, other, contextPtr); }
    bool operator <=(const QGen &other) { return giac::is_greater(other, *this, contextPtr); }
    bool operator >=(const QGen &other) { return giac::is_greater(*this, other, contextPtr); }

    /*
     * Binary operations
     */

    QGen makeUnion(const QGen &other) const { return makeSymb(giac::at_union, seq(*this, other)); }
    QGen makeIntersection(const QGen &other) const { return makeSymb(giac::at_intersect, seq(*this, other)); }
    QGen makeSetDifference(const QGen &other) const { return makeSymb(giac::at_minus, seq(*this, other)); }
    QGen makeCrossProduct(const QGen &other) const { return makeSymb(giac::at_cross, seq(*this, other)); }
    QGen makeInterval(const QGen &other) const { return makeSymb(giac::at_interval, seq(*this, other)); }
    QGen makeBinomial(const QGen &other) const { return makeSymb(giac::at_binomial, seq(*this, other)); }
    QGen makeEqualTo(const QGen &other) const { return makeSymb(giac::at_equal, seq(*this, other)); }
    QGen makeLessThan(const QGen &other) const { return makeSymb(giac::at_inferieur_strict, seq(*this, other)); }
    QGen makeGreaterThan(const QGen &other) const { return makeSymb(giac::at_superieur_strict, seq(*this, other)); }
    QGen makeLessOrEqualTo(const QGen &other) const { return makeSymb(giac::at_inferieur_egal, seq(*this, other)); }
    QGen makeGreaterOrEqualTo(const QGen &other) const { return makeSymb(giac::at_superieur_egal, seq(*this, other)); }
    QGen makePower(const QGen &other) const { return makeSymb(giac::at_pow, seq(*this, other)); }
    QGen makeCompositionPower(const QGen &other) const { return makeSymb(giac::at_composepow, seq(*this, other)); }
    QGen makeComposition(const QGen &other) const { return makeSymb(giac::at_compose, seq(*this, other)); }

    /*
     * Unary operations
     */

    QGen makeRoot(int n) const { return makePower(QGen(makeSymb(giac::at_inv, n), contextPtr)); }
    QGen makeSquareRoot() const { return makeRoot(2); }
    QGen makeNegative() const { return makeSymb(giac::at_neg, *this); }
    QGen makeFactorial() const { return makeSymb(giac::at_factorial, *this); }
    QGen makeAbsoluteValue() const { return makeSymb(giac::at_abs, *this); }

    /*
     * Basic functions
     */

    // absolute value of this expression
    QGen absoluteValue() const { return QGen(giac::abs(*this, contextPtr), contextPtr); }
    // power for given exponent
    QGen pow(const QGen &exponent) const { return QGen(giac::_pow(seq(*this, exponent), contextPtr), contextPtr); }
    // n-th power
    QGen pow(int n) const { return pow(QGen(giac::gen(n), contextPtr)); }

    /*
     * Equation and inequation solving
     */

    QGen solve(const QGen &unknown) const;
    QGen approxSolve(const QGen &unknown, const QGen &guess) const;

    /*
     * Calculus functions
     */

    // n-th derivative w.r.t. var
    QGen derive(const QGen &var, int n) const;
    // function derivative w.r.t. var
    QGen functionalDerive(const QGen &var) const;
    // Riemann integral on [a,b] w.r.t. var
    QGen integrate(const QGen &var, const QGen &a, const QGen &b, bool approx = false) const;
    // limit when var->dest with optional direction (-1: from left, 1: from right)
    QGen limit(const QGen &var, const QGen &dest, int dir = 0) const;
};


#endif // QGEN_H
