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

QGen QGen::identifier(const QString &name, const giac::context *ctx)
{
    return QGen(giac::identificateur(name.toStdString().c_str()), ctx);
}

QGen QGen::string(const QString &text, const giac::context *ctx)
{
    return QGen(giac::string2gen(text.toStdString()), ctx);
}

QGen QGen::fraction(int numerator, int denominator, const giac::context *ctx)
{
    return QGen(giac::fraction(numerator, denominator), ctx);
}

QGen QGen::binomial(int n, int k, const giac::context *ctx)
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

QGen QGen::makeSymb(const giac::unary_function_ptr *p, const giac::gen &args) const
{
    return QGen(giac::symbolic(p, args), contextPtr);
}

QString QGen::toString() const
{
    std::stringstream ss;
    ss << (giac::gen)*this;
    return QString(ss.str().data());
}

bool QGen::hasMinusSign() const
{
    return  (isSymbolic() && isMinus()) || (isInteger() && integerValue() < 0) ||
            (isApproximate() && doubleValue() < 0) || (isFraction() && fractionNumerator() > 0);
}

bool QGen::isElementaryFunction() const {
    return  isSommet(giac::at_exp) || isSommet(giac::at_ln) || isSommet(giac::at_log10) ||
            isSommet(giac::at_sin) || isSommet(giac::at_cos) || isSommet(giac::at_tan) ||
            isSommet(giac::at_sec) || isSommet(giac::at_csc) || isSommet(giac::at_cot) ||
            isSommet(giac::at_asin) || isSommet(giac::at_acos) || isSommet(giac::at_atan) ||
            isSommet(giac::at_acot) || isSommet(giac::at_sinh) || isSommet(giac::at_cosh) ||
            isSommet(giac::at_tanh) || isSommet(giac::at_asinh) ||
            isSommet(giac::at_acosh) || isSommet(giac::at_atanh);
}

bool QGen::isRational() const
{
    if (isReciprocal())
        return true;
    if (isProduct())
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

bool QGen::isAssociativeOperator() const
{
    return  isSummation() || isProduct() || isUnion() || isIntersection() ||
            isConjunction() || isDisjunction() || isExclusiveOr() || isComposition() ||
            isHadamardSummation() || isHadamardProduct() || isHadamardDifference();
}

bool QGen::isUnaryOperator() const
{
    return  isNegation() || isMinus() || isDerivative() || isFactorial() ||
            isIntegral() || isReciprocal() || isIncrement() || isDecrement();
}

bool QGen::isBinaryOperator() const
{
    return  isPower() || isEqual() || isDifferent() || isLessThan() || isGreaterThan() ||
            isLessThanOrEqualTo() || isGreaterThanOrEqualTo() || isSetDifference() || isDefinition() ||
            isCompositionPower() || isCrossProduct() || isHadamardPower() || isHadamardDivision();
}

bool QGen::isInequalityOperator() const
{
    return  isEquation() || isLessThan() || isGreaterThan() || isLessThanOrEqualTo() || isGreaterThanOrEqualTo();
}

QGen::Vector QGen::flattenOperands() const
{
    Q_ASSERT(isSymbolic());
    Vector operands;
    QGen feuille = getFeuille();
    if (!feuille.isVector())
        operands.push_back(feuille);
    else
    {
        Vector feuilleVector = feuille.toVector();
        Vector::iterator it;
        for (int i = 0 ; i < feuilleVector.length(); ++i) {
            QGen operand = feuilleVector.at(i);
            if (operand.isSommet(getSommet())) {
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
            vector.append(QGen(*it, contextPtr));
    }
    return vector;
}

qreal QGen::approximateValue() const
{
    giac::gen e(giac::_evalf(*this, contextPtr));
    return e.is_approx() ? e.DOUBLE_val() : 0;
}

bool QGen::getModularComponents(QGen &value, QGen &modulus) const
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

giac::gen QGen::seq(const giac::gen &g1, const giac::gen &g2) const
{
    return giac::makesequence(g1, g2);
}

giac::gen QGen::seq(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3) const
{
    return giac::makesequence(g1, g2, g3);
}

giac::gen QGen::seq(const giac::gen &g1, const giac::gen &g2, const giac::gen &g3, const giac::gen &g4) const
{
    return giac::makesequence(g1, g2, g3, g4);
}

QGen QGen::derive(const QGen &var, int n) const
{
    return QGen(giac::_derive(seq(*this, var, giac::gen(n)), contextPtr), contextPtr);
}

QGen QGen::functionalDerive(const QGen &var) const
{
    return QGen(giac::_function_diff(seq(*this, var), contextPtr), contextPtr);
}

QGen QGen::integrate(const QGen &var, const QGen &a, const QGen &b, bool approx) const
{
    return QGen(giac::_integrate(seq(*this, var, a, b), contextPtr), contextPtr);
}

QGen QGen::limit(const QGen &var, const QGen &dest, int dir) const
{
    giac::gen args = seq(*this, var, dest);
    if (dir != 0)
        args._VECTptr->push_back(dir);
    return QGen(giac::_limit(args, contextPtr), contextPtr);
}

QGen QGen::solve(const QGen &unknown) const
{
    return QGen(giac::_solve(seq(*this, unknown), contextPtr));
}

QGen QGen::approxSolve(const QGen &unknown, const QGen &guess) const
{
    return QGen(giac::_fsolve(seq(*this, unknown, guess), contextPtr));
}
