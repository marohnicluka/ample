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

#ifndef EQUATION_H
#define EQUATION_H

#include <QPicture>
#include <giac/config.h>
#include <giac/giac.h>
#include "glyphs.h"

class Equation : public QPicture
{
    const giac::context *contextptr;
    giac::gen expression;
    Glyphs glyphs;
    Glyphs bigGlyphs;
    QString fontFamily;
    int fontSize;
    int leftBracketType;
    int rightBracketType;

    int type(const giac::gen &g) { return g.type; }
    int type() { return type(expression); }
    int subType(const giac::gen &g) { Q_ASSERT(isVector(g)); return g.subtype; }
    int subType() { return subType(expression); }

    bool isString(const giac::gen &g) { return type(g) == giac::_STRNG; }
    bool isString() { return isString(expression); }
    bool isIdentifier(const giac::gen &g) { return type(g) == giac::_IDNT; }
    bool isIdentifier() { return isString(expression); }
    bool isVector(const giac::gen &g) { return type(g) == giac::_VECT; }
    bool isVector() { return isVector(expression); }
    bool isSymbolic(const giac::gen &g) { return type(g) == giac::_SYMB; }
    bool isSymbolic() { return isSymbolic(expression); }
    bool isMap(const giac::gen &g) { return type(g) == giac::_MAP; }
    bool isMap() { return isMap(expression); }
    bool isNegative(const giac::gen &g);
    bool isNegative() { return isNegative(expression); }
    bool isFraction(const giac::gen &g) { return type(g) == giac::_FRAC; }
    bool isFraction() { return isFraction(expression); }
    bool isInteger(const giac::gen &g) { return g.is_integer(); }
    bool isInteger() { return isInteger(expression); }
    bool isDouble(const giac::gen &g) { return g.is_approx(); }
    bool isDouble() { return isDouble(expression); }
    bool isNumber(const giac::gen &g) { return isFraction(g) || isInteger(g) || isDouble(g); }
    bool isNumber() { return isNumber(expression); }
    bool isElementaryFunction(const giac::gen &g);
    bool isElementaryFunction() { return isElementaryFunction(expression); }
    bool isSommet(const giac::gen &g, const giac::unary_function_ptr *funcPtr) { return g.is_symb_of_sommet(funcPtr); }
    bool isSommet(const giac::unary_function_ptr *funcPtr) { return isSommet(expression, funcPtr); }

    const giac::gen fractionNumerator(const giac::gen &g) { Q_ASSERT(isFraction()); return g._FRACptr->num; }
    const giac::gen fractionNumerator() { return fractionNumerator(expression); }
    const giac::gen fractionDenominator(const giac::gen &g) { Q_ASSERT(isFraction()); return g._FRACptr->den; }
    const giac::gen fractionDenominator() { return fractionDenominator(expression); }
    const giac::vecteur assocOperands(const giac::gen &g);
    const giac::unary_function_ptr getSommet(const giac::gen &g) { Q_ASSERT(isSymbolic(g)); return g._SYMBptr->sommet; }
    const giac::unary_function_ptr getSommet() { return getSommet(expression); }
    const giac::vecteur getFeuille(const giac::gen &g) { Q_ASSERT(isSymbolic(g)); return *g._SYMBptr->feuille._VECTptr; }
    const giac::vecteur getFeuille() { return getFeuille(expression); }
    const giac::gen firstOperand(const giac::gen &g) { Q_ASSERT(!getFeuille(g).empty()); return getFeuille(g).front(); }
    const giac::gen firstOperand() { return firstOperand(expression); }
    const giac::gen secondOperand(const giac::gen &g) { Q_ASSERT(getFeuille(g).size() > 1); return getFeuille(g).at(1); }
    const giac::gen secondOperand() { return secondOperand(expression); }
    const giac::vecteur toVector(const giac::gen &g) { Q_ASSERT(isVector(g)); return *g._VECTptr; }
    const giac::vecteur toVector() { return toVector(expression); }
    giac::gen absoluteValue(const giac::gen &g) { return giac::abs(g); }
    giac::gen absoluteValue() { return absoluteValue(expression); }
    giac::vecteur assocOperands() { return assocOperands(expression); }

    QString toString(const giac::gen &g);
    QString toString() { return toString(expression); }
    QString sommetName(const giac::gen &g) { Q_ASSERT(isSymbolic(g)); return getSommet(g).ptr()->print(contextptr); }
    QString sommetName() { return sommetName(expression); }

    QString paddedString(QString str, bool padLeft = true, bool padRight = true);
    QString numberToSuperscript(int integer, bool parens = false);
    QString numberToSubscript(int integer, bool parens = false);

    QPointF renderText(QString text, QPointF where = QPointF(0.0,0.0));

    void renderString();
    void renderNumber();
    void renderIdentifier();
    void renderFunction(int exponent = 0);
    void renderVector();
    void renderModulo();
    void renderMap();
    void renderSymbolic();
    void renderSequence(const giac::vecteur &operands, const QString &separator, bool isAddition = false);
    void renderFraction(const giac::gen &numerator, const giac::gen &denominator);
    void renderPower(const giac::gen &base, const giac::gen &exponent);
    void renderRoot(const giac::gen &arg, int degree);

public:
    enum BracketType
    {
        None,
        Parenthesis,
        SquareBracket,
        WhiteSquareBracket,
        CurlyBracket,
        WhiteCurlyBracket,
        AngleBracket,
        FloorBracket,
        CeilBracket,
        AbsoluteValueBracket
    };

    Equation(const giac::gen &e, const giac::context *ctx, const QString &family, int size);
    void setBracketType(int leftType, int rightType = -1);
    void render();

    int width() { return boundingRect().width(); }
    int height() { return boundingRect().height(); }
    int ascent() { return -boundingRect().y(); }
    int descent() { return height() - ascent(); }
};

#endif // EQUATION_H
