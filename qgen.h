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

#include <QPicture>
#include <QStringList>
#include <QCoreApplication>
#include <QPainter>
#include <sstream>
#include <giac/config.h>
#include <giac/giac.h>
#include "mathglyphs.h"

typedef giac::unary_function_ptr GiacFunctionPointer;

class QGen : public giac::gen
{

public:
    typedef QList<QGen> Vector;

private:
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

    const QGen typeError(const QString &message);

    static QString paddedString(QString str, bool padLeft = true, bool padRight = true);
    static QString numberToSuperscript(int integer, bool parens = false);
    static QString numberToSubscript(int integer, bool parens = false);

    QPointF putText(QString text, QPointF where = QPointF(0.0,0.0));

    QPicture renderString();
    QPicture renderNumber();
    QPicture renderIdentifier();
    QPicture renderFunction(int exponent = 0);
    QPicture renderVector();
    QPicture renderModulo();
    QPicture renderMap();
    QPicture renderSymbolic();
    QPicture renderUnaryOperation();
    QPicture renderBinaryOperation();
    QPicture renderAssociativeOperation();
    QPicture renderFraction(const QGen &numerator, const QGen &denominator);
    QPicture renderPower(const QGen &base, const QGen &exponent);
    QPicture renderRoot(const QGen &argument, int degree);

    giac::gen qString2gen(const QString &str) { return giac::gen(str.toStdString().c_str(), contextPtr); }

    QString tr(const char *text) { return QCoreApplication::translate("MathDisplay", text); }

public:
    const giac::context *contextPtr;

    QGen(const QGen &e)
        : giac::gen(e), contextPtr(e.contextPtr) { }
    QGen(const giac::gen &e, const giac::context *ctx)
        : giac::gen(e), contextPtr(ctx) { }
    QGen(const QString &expression, const giac::context *ctx)
        : giac::gen(expression.toStdString().c_str(), ctx), contextPtr(ctx) { }

    QString toString();
    QString toLaTeX();
    QString toMathML();
    QPicture toPicture(const QString &fontFamily, int fontSize);

    bool isString() { return type == giac::_STRNG; }
    bool isIdentifier() { return type == giac::_IDNT; }
    bool isIdentifierWithName(const QString &name) { return is_identificateur_with_name(name.toStdString().c_str()); }
    bool isVector() { return type == giac::_VECT; }
    bool isVectorOfLength(int length) { return is_vector_of_size(length); }
    bool isSequenceVector() { return isVector() && subtype == giac::_SEQ__VECT; }
    bool isSetVector() { return isVector() && subtype == giac::_SET__VECT; }
    bool isAssumeVector() { return isVector() && subtype == giac::_ASSUME__VECT; }
    bool isPolynomialVector() { return isVector() && subtype == giac::_POLY1__VECT; }
    bool isMatrixVector() { return isVector() && subtype == giac::_MATRIX__VECT; }
    bool isLineVector() { return isVector() && subtype == giac::_LINE__VECT; }
    bool isListVector() { return isVector() && subtype == giac::_LIST__VECT; }
    bool isSymbolic() { return type == giac::_SYMB; }
    bool isMap() { return type == giac::_MAP; }
    bool isConstant() { return is_constant(); }
    bool isFraction() { return type == giac::_FRAC; }
    bool isInteger() { return is_integer(); }
    bool isExact() { return giac::is_exact(*this); }
    bool isApproximate() { return is_approx(); }
    bool isExplicitNumber() { return isFraction() || isInteger() || isApproximate(); }
    bool isNumber() { return isConstant() || isExplicitNumber(); }
    bool isModular() { return type == giac::_MOD; }
    bool isMatrix() { return isMatrixVector() || giac::ckmatrix(*this); }
    bool isFunction() { return type == giac::_FUNC; }
    bool isElementaryFunction();
    bool isUndefined() { return giac::is_undef(*this); }
    bool isInfinite() { return giac::is_inf(*this); }
    bool isZero() { return giac::is_zero(*this, contextPtr); }
    bool isExactlyZero() { return giac::is_exactly_zero(*this); }
    bool isVectorOfZeros() { return isVector() ? giac::is_zero__VECT(*this->_VECTptr, contextPtr) : false; }
    bool isOne() { return giac::is_one(*this); }
    bool isMinusOne() { return giac::is_minus_one(*this); }
    bool isNonnegative() { return giac::is_positive(*this, contextPtr); }
    bool isNegative() { return !isNonnegative(); }
    bool isPositive() { return giac::is_strictly_positive(*this, contextPtr); }
    bool hasMinusSign();

    bool isSommet(const GiacFunctionPointer *f) { return is_symb_of_sommet(f); }
    GiacFunctionPointer *getSommet() { return isSymbolic() ? &_SYMBptr->sommet : NULL; }
    QString sommetName() { return isSymbolic() ? getSommet()->ptr()->print(contextPtr) : ""; }
    QGen getFeuille() { return isSymbolic() ? QGen(_SYMBptr->feuille, contextPtr)
                                            : typeError(tr("Not a symbolic expression")); }

    bool isPolynomialRoot() { return isSommet(giac::at_rootof); }

    bool isSummation() { return isSommet(giac::at_plus); }
    bool isProduct() { return isSommet(giac::at_prod); }
    bool isPower() { return isSommet(giac::at_pow); }
    bool isReciprocal() { return isSommet(giac::at_inv); }
    bool isConjunction() { return isSommet(giac::at_and); }
    bool isDisjunction() { return isSommet(giac::at_ou); }
    bool isExclusiveOr() { return isSommet(giac::at_xor); }
    bool isNegation() { return isSommet(giac::at_not); }
    bool isMinusSign() { return isSommet(giac::at_neg); }
    bool isEqual() { return isSommet(giac::at_same); }
    bool isDifferent() { return isSommet(giac::at_different); }
    bool isLessThan() { return isSommet(giac::at_inferieur_strict); }
    bool isGreaterThan() { return isSommet(giac::at_superieur_strict); }
    bool isLessThanOrEqualTo() { return isSommet(giac::at_inferieur_egal); }
    bool isGreaterThanOrEqualTo() { return isSommet(giac::at_superieur_egal); }
    bool isIncrement() { return isSommet(giac::at_increment); }
    bool isDecrement() { return isSommet(giac::at_decrement); }
    bool isUnion() { return isSommet(giac::at_union); }
    bool isIntersection() { return isSommet(giac::at_intersect); }
    bool isSetMinus() { return isSommet(giac::at_minus); }
    bool isFactorial() { return isSommet(giac::at_factorial); }
    bool isDerivative() { return isSommet(giac::at_derive); }
    bool isIntegral() { return isSommet(giac::at_integrate); }
    bool isIfThenElse() { return isSommet(giac::at_when); }
    bool isDefinition() { return isSommet(giac::at_sto); }
    bool isComposition() { return isSommet(giac::at_compose); }
    bool isCompositionPower() { return isSommet(giac::at_composepow); }
    bool isEquality() { return isSommet(giac::at_equal); }
    bool isCrossProduct() { return isSommet(giac::at_cross); }
    bool isInterval() { return isSommet(giac::at_interval); }
    bool isBinomial() { return isSommet(giac::at_binomial); }

    bool isAssociativeOperation();
    bool isUnaryOperation();
    bool isBinaryOperation();
    bool isTernaryOperation();
    bool isRelation();

    int integerValue() { return isInteger() ? this->val : 0; }
    qreal doubleValue() { return isApproximate() ? this->DOUBLE_val() : 0; }
    qreal approximateValue() { giac::gen e(giac::_evalf(*this, contextPtr)); return e.is_approx() ? e.DOUBLE_val() : 0; }
    QString stringValue() { return isString() ? this->_STRNGptr->data() : ""; }
    int fractionNumerator() { return isFraction() ? _FRACptr->num.val : 0; }
    int fractionDenominator() { return isFraction() ? _FRACptr->den.val : 0; }
    QGen absoluteValue() { return QGen(giac::abs(*this, contextPtr), contextPtr); }

    Vector assocOperands();
    Vector toVector();
    int nOperands() { return isSymbolic() && getFeuille().isVector() ? int(getFeuille().toVector().size()) : -1; }
    QGen getOperand(int n = 0);

    QGen operator +(const QGen &other) { return QGen((giac::gen)*this + other, contextPtr); }
    QGen operator +(int other) { return QGen((giac::gen)*this + giac::gen(other), contextPtr); }
    QGen operator +(qreal other) { return QGen((giac::gen)*this + giac::gen(other), contextPtr); }

    QGen operator -(const QGen &other) { return QGen((giac::gen)*this - other, contextPtr); }
    QGen operator -(int other) { return QGen((giac::gen)*this - giac::gen(other), contextPtr); }
    QGen operator -(qreal other) { return QGen((giac::gen)*this - giac::gen(other), contextPtr); }

    QGen operator *(const QGen &other) { return QGen((giac::gen)*this * other, contextPtr); }
    QGen operator *(int other) { return QGen((giac::gen)*this * giac::gen(other), contextPtr); }
    QGen operator *(qreal other) { return QGen((giac::gen)*this * giac::gen(other), contextPtr); }

    QGen operator /(const QGen &other) { return QGen((giac::gen)*this / other, contextPtr); }
    QGen operator /(int other) { return QGen((giac::gen)*this / giac::gen(other), contextPtr); }
    QGen operator /(qreal other) { return QGen((giac::gen)*this / giac::gen(other), contextPtr); }

    QGen& operator =(const QGen &other) { giac::gen::operator =(other); return *this; }
    QGen& operator =(int number) { giac::gen::operator =(giac::gen(number)); return *this; }
    QGen& operator =(qreal number) { giac::gen::operator =(giac::gen(number)); return *this; }
    QGen& operator =(const QString &s) { giac::gen::operator =(qString2gen(s)); return *this; }

    QGen& operator +=(const QGen &other) { giac::gen::operator =((giac::gen)*this + other); return *this; }
    QGen& operator +=(int number) { giac::gen::operator =((giac::gen)*this + giac::gen(number)); return *this; }
    QGen& operator +=(qreal number) { giac::gen::operator =((giac::gen)*this + giac::gen(number)); return *this; }

    QGen& operator -=(const QGen &other) { giac::gen::operator =((giac::gen)*this - other); return *this; }
    QGen& operator -=(int number) { giac::gen::operator =((giac::gen)*this - giac::gen(number)); return *this; }
    QGen& operator -=(qreal number) { giac::gen::operator =((giac::gen)*this - giac::gen(number)); return *this; }

    QGen& operator *=(const QGen &other) { giac::gen::operator =((giac::gen)*this * other); return *this; }
    QGen& operator *=(int number) { giac::gen::operator =((giac::gen)*this * giac::gen(number)); return *this; }
    QGen& operator *=(qreal number) { giac::gen::operator =((giac::gen)*this * giac::gen(number)); return *this; }

    QGen& operator /=(const QGen &other) { giac::gen::operator =((giac::gen)*this / other); return *this; }
    QGen& operator /=(int number) { giac::gen::operator =((giac::gen)*this / giac::gen(number)); return *this; }
    QGen& operator /=(qreal number) { giac::gen::operator =((giac::gen)*this / giac::gen(number)); return *this; }

    bool operator <(const QGen &other) { return giac::is_strictly_greater(other, *this, contextPtr); }
    bool operator >(const QGen &other) { return giac::is_strictly_greater(*this, other, contextPtr); }
    bool operator <=(const QGen &other) { return giac::is_greater(other, *this, contextPtr); }
    bool operator >=(const QGen &other) { return giac::is_greater(*this, other, contextPtr); }
};


#endif // QGEN_H
