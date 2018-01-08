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
#include <QStack>
#include <QVector>
#include <QFont>
#include <QPainter>
#include <QFlags>
#include <QRegularExpression>
#include <qmath.h>
#include <giac/config.h>
#include <giac/giac.h>
#include "mathglyphs.h"

using namespace giac;

class QGen
{
    gen *expr;
    const context *ct;
    bool owns;

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

    enum BracketType { None, Parenthesis, WhiteParenthesis, SquareBracket, WhiteSquareBracket,
                       CurlyBracket, WhiteCurlyBracket, FloorBracket, CeilingBracket,
                       AngleBracket, StraightBracket, DoubleStraightBracket };
    enum AccentType { Hat, Check, Tilde, Acute, Grave, Dot, DoubleDot, TripleDot, Bar, Vec };
    enum MathPadding { Hair, Thin, Medium, Thick };

    class Display : public QPicture
    {
        const gen *m_expression;
        bool m_grouped;
        bool m_requiresMinusSign;
        int m_priority;

    public:
        Display()
            : QPicture()
            , m_expression(NULL)
            , m_grouped(false)
            , m_requiresMinusSign(false)
            , m_priority(0) { }

        Display(const QGen &expr)
            : QPicture()
            , m_expression(&expr.expression())
            , m_grouped(false)
            , m_requiresMinusSign(false)
            , m_priority(0) { }

        Display(const QPicture &picture)
            : QPicture(picture)
            , m_expression(NULL)
            , m_grouped(false)
            , m_requiresMinusSign(false)
            , m_priority(0) { }

        Display(const Display &display)
            : QPicture(display)
            , m_expression(display.expression())
            , m_grouped(display.isGrouped())
            , m_requiresMinusSign(display.isMinusSignRequired())
            , m_priority(display.priority()) { }

        int priority() const { return m_priority; }
        bool isGrouped() const { return m_grouped; }
        bool isMinusSignRequired() const { return m_requiresMinusSign; }
        const gen *expression() const { return m_expression; }
        void setPriority(int value) { m_priority = value; }
        void setGrouped(bool yes) { m_grouped = yes; }
        void requireMinusSign(bool yes) { m_requiresMinusSign = yes; }
        void linkWithExpression(const QGen &g) { m_expression = &g.expression(); }
        int leftBearing() const { return -boundingRect().x(); }
        int advance() const { return boundingRect().width() - leftBearing(); }
        int ascent() const { return -boundingRect().y(); }
        int descent() const { return boundingRect().height() - ascent(); }
        int totalWidth() const { return boundingRect().width(); }
        int totalHeight() const { return boundingRect().height(); }
    };

    struct UserOperator
    {
        const context *ct;
        OperatorPriority priority;
        QString symbol;
        bool isRelation;
        bool isAssociative;
        bool isCommutative;
    };

    static QMap<QString, UserOperator> userOperators;

    static gen seq(const gen &g1, const gen &g2);
    static gen seq(const gen &g1, const gen &g2, const gen &g3);
    static gen seq(const gen &g1, const gen &g2, const gen &g3, const gen &g4);

    static gen vec(const gen &g);
    static gen vec(const gen &g1, const gen &g2);
    static gen vec(const gen &g1, const gen &g2, const gen &g3);
    static gen vec(const gen &g1, const gen &g2, const gen &g3, const gen &g4);

    QGen makeSymb(const unary_function_ptr *p, const gen &args) const;
    vecteur flattenOperands() const;

    unary_function_ptr &sommet() const { return expr->_SYMBptr->sommet; }
    gen &feuille() const { return expr->_SYMBptr->feuille; }
    vecteur *feuilleVector() const { return expr->_SYMBptr->feuille._VECTptr; }

    // RENDERING

    static QList<int> fontSizes;
    static QList<QFont> fonts;
    static QString fontFamily;

    QStack<int> fontSizeLevelStack;
    bool renderFontItalic;
    bool renderFontBold;
    int fractionDepth;

    int fontSizeLevel() const { Q_ASSERT(!fontSizeLevelStack.empty()); return fontSizeLevelStack.top(); }
    int smallerFontSizeLevel() const { return fontSizeLevel() - 1; }
    int largerFontSizeLevel() const { return fontSizeLevel() + 1; }

    void setRenderingItalic(bool yes) { renderFontItalic = yes; }
    void setRenderingBold(bool yes) { renderFontBold = yes; }

    static QString paddedText(const QString &text, MathPadding padding = Medium, bool padLeft = true, bool padRight = true);
    static QString quotedText(const QString &text);
    static QString numberToSuperscriptText(int integer, bool parens = false);
    static QString numberToSubscriptText(int integer, bool parens = false);
    static QString identifierStringToUnicode(const QString &text, bool bold, bool italic);

    void render(Display &dest, const QGen &g, int sizeLevel = 0);
    Display renderNormal(const QGen &g);
    Display renderSmaller(const QGen &g);
    Display renderLarger(const QGen &g);

    void renderHorizontalLine(QPainter &painter, qreal x, qreal y, qreal length, qreal widthFactor = 1.0);
    void renderHorizontalLine(Display &dest, QPointF where, qreal length, qreal widthFactor = 1.0);
    qreal renderText(Display &dest, const QString &text, int relativeFontSizeLevel = 0,
                     QPointF where = QPointF(0, 0), QRectF *boundingRect = Q_NULLPTR);
    void renderTextAndAdvance(Display &dest, const QString &text, QPointF &penPoint);
    void renderBracketExtensionFill(QPainter &painter, const QChar &extension, qreal x, qreal yLower, qreal yUpper);
    qreal renderSingleBracket(QPainter &painter, qreal x, qreal halfHeight, QChar bracket, bool scaleX = false);
    qreal renderSingleFillBracket(QPainter &painter, qreal x, qreal halfHeight,
                                  QChar upperPart, QChar lowerPart, QChar extension, QChar singleBracket);
    qreal renderCurlyBracket(QPainter &painter, qreal x, qreal halfHeight, bool left);
    void renderDisplay(Display &dest, const Display &source, QPointF where);
    void renderDisplayWithRadical(Display &dest, const Display &source, QPointF where);
    void renderDisplayWithAccent(Display &dest, const Display &source, AccentType accentType, QPointF where);
    qreal renderDisplayWithBrackets(Display &dest, const Display &source,
                                    BracketType leftBracketType, BracketType rightBracketType, QPointF where);
    qreal renderDisplayWithParentheses(Display &dest, const Display &source, QPointF where);
    qreal renderDisplayWithPriority(Display &dest, const Display &source, int priority, QPointF where);
    qreal renderDisplayWithSquareBrackets(Display &dest, const Display &source, QPointF where);
    qreal renderDisplayWithCurlyBrackets(Display &dest, const Display &source, QPointF where);
    void renderDisplayAndAdvance(Display &dest, const Display &source, QPointF &penPoint);
    void renderRealNumber(Display &dest, const QGen &g, QPointF where);
    void renderComplexNumber(Display &dest, const QGen &realPart, const QGen &imaginaryPart, QPointF where);
    void renderIdentifier(Display &dest, const QGen &g, QPointF where);
    void renderLeadingUnderscoreIdentifier(Display &dest, const QGen &g, QPointF where);
    void renderVector(Display &dest, const QGen &g, QPointF where);
    void renderModular(Display &dest, const QGen &g, QPointF where);
    void renderMap(Display &dest, const QGen &g, QPointF where);
    void renderSymbolic(Display &dest, const QGen &g, QPointF where);
    void renderUnary(Display &dest, const QGen &g, QPointF where);
    void renderBinary(Display &dest, const QGen &g, QPointF where);
    void renderAssociative(Display &dest, const QGen &g, const QVector<QGen> &operands, QPointF where);
    void renderFraction(Display &dest, const QGen &numerator, const QGen &denominator, QPointF where);
    void renderSuperscript(Display &dest, const QGen &base, const QGen &exponent, int priority,
                           QPointF where, bool withCircle = false);
    void renderSubscript(Display &dest, const QGen &base, const QGen &subscript, int priority, QPointF where);

    void movePenPointX(QPointF &penPoint, qreal offset) { penPoint.setX(penPoint.x() + offset); }
    void movePenPointY(QPointF &penPoint, qreal offset) { penPoint.setY(penPoint.y() + offset); }
    void movePenPointXY(QPointF &penPoint, qreal offsetX, qreal offsetY)
    {
        movePenPointX(penPoint, offsetX);
        movePenPointY(penPoint, offsetY);
    }

    void storeBoundingRect(gen &g, int x, int y, int width, int height);
    void translateBoundingRect(gen &g, int dx, int dy);
    void setSelected(gen &g, bool yes) { g._EQWptr->selected = yes; }

    const QFont &getFont(int fontSizeLevel);
    qreal textWidth(const QString &text, int relativeFontSizeLevel = 0);
    QRectF textTightBoundingRect(const QString &text, int relativeFontSizeLevel = 0);
    qreal fontHeight(int relativeFontSizeLevel = 0);
    qreal fontAscent(int relativeFontSizeLevel = 0);
    qreal fontDescent(int relativeFontSizeLevel = 0);
    qreal fontXHeight(int relativeFontSizeLevel = 0);
    qreal fontMidLine(int relativeFontSizeLevel = 0);
    qreal fontLeading(int relativeFontSizeLevel = 0);
    qreal lineWidth(int relativeFontSizeLevel = 0);
    qreal linePadding(int relativeSizeLevel = 0);

public:
    enum InequalityType { LessThan = -2, GreaterThan = 2, LessThanOrEqualTo = -1, GreaterThanOrEqualTo = 1 };
    enum OperatorType { Associative = 1, Unary = 2, Binary = 3, Ternary = 4 };
    enum AlignmentFlag { AlignLeft = 1, AlignRight = 2, AlignTop = 4, AlignBottom = 8,
                          AlignBaseline = 16, AlignHCenter = 32, AlignVCenter = 64 };
    Q_DECLARE_FLAGS(Alignment, AlignmentFlag)

    QGen(const gen &e, GIAC_CONTEXT = context0);
    QGen(gen *e, GIAC_CONTEXT = context0);
    QGen(const QGen &e);
    QGen(GIAC_CONTEXT = context0);
    QGen(const QString &text, GIAC_CONTEXT = context0);
    QGen(int value, GIAC_CONTEXT = context0);
    QGen(qreal value, GIAC_CONTEXT = context0);

    ~QGen();

    static QGen identifier(const QString &name, GIAC_CONTEXT = context0);
    static QGen string(const QString &text, GIAC_CONTEXT = context0);
    static QGen fraction(int numerator, int denominator, GIAC_CONTEXT = context0);
    static QGen binomial(const QGen &n, const QGen &k, GIAC_CONTEXT = context0);
    static QGen plusInfinity(GIAC_CONTEXT = context0);
    static QGen minusInfinity(GIAC_CONTEXT = context0);
    static QGen undefined(GIAC_CONTEXT = context0);
    static QGen equation(const QGen &left, const QGen &right, GIAC_CONTEXT = context0);
    static bool defineBinaryOperator(const QString &name, OperatorPriority priority, const QString &symbol,
                                     const QGen &realFunction, bool checkProperties,
                                     GIAC_CONTEXT = context0);
    static bool defineBinaryRelation(const QString &name, OperatorPriority priority, const QString &symbol,
                                     const QGen &booleanFunction, GIAC_CONTEXT = context0);
    static bool findUserOperator(const QString &name, UserOperator &properties);
    static void setRenderingFont(const QString &family = "FreeSerif", int basePointSize = 12);

    gen &expression() const { return *expr; }
    const context *contextPtr() const { return ct; }

    QString toString() const { return QString(expr->print(ct).data()); }
    QString toLaTeX() const;
    QString toMathML() const;
    QPicture toPicture(const QString &family, int size) const;

    int type() const { return expr->type; }
    int subtype() const { return expr->subtype; }
    bool isUnaryFunction(const unary_function_ptr *f) const { return expr->is_symb_of_sommet(f); }
    QString unaryFunctionName() const { return isSymbolic() ? sommet().ptr()->print(ct) : ""; }
    QGen unaryFunctionArgument() const { return isSymbolic() ? QGen(&feuille(), ct) : undefined(); }

    bool isString() const { return type() == _STRNG; }
    bool isIdentifier() const { return type() == _IDNT; }
    bool isPlaceholderIdentifier() const { return isIdentifier() && toString().startsWith("` "); }
    bool isDoubleLetterIdentifier() const;
    bool isInfinityIdentifier() const { return isIdentifier() && *expr->_IDNTptr == _IDNT_infinity(); }
    bool isUndefIdentifier() const { return isIdentifier() && *expr->_IDNTptr == _IDNT_undef(); }
    bool isIdentifierWithName(const QString &name) const;
    bool isVector() const { return type() == _VECT; }
    bool isVectorOfLength(int length) const { return expr->is_vector_of_size(length); }
    bool isOrdinaryVector() const { return isVector() && subtype() == 0; }
    bool isSequenceVector() const { return isVector() && subtype() == _SEQ__VECT; }
    bool isSetVector() const { return isVector() && subtype() == _SET__VECT; }
    bool isListVector() const { return isVector() && subtype() == _LIST__VECT; }
    bool isPolynomVector() const { return isVector() && subtype() == _POLY1__VECT; }
    bool isMatrixVector() const { return isVector() && subtype() == _MATRIX__VECT; }
    bool isAssumeVector() const { return isVector() && subtype() == _ASSUME__VECT; }
    bool isSymbolic() const { return type() == _SYMB; }
    bool isMap() const { return type() == _MAP; }
    bool isFraction() const { return type() == _FRAC; }
    bool isFraction(QGen &numerator, QGen &denominator) const;
    bool isConstant() const { return expr->is_constant() || isFraction(); }
    bool isMultiPrecisionFloat() const { return type() == _REAL; }
    bool isFloat() const { return type() == _FLOAT_; }
    bool isDouble() const { return type() == _DOUBLE_; }
    bool isFloatingPointNumber() const { return isDouble() || isFloat() || isMultiPrecisionFloat(); }
    bool isComplex() const { return type() == _CPLX; }
    bool isComplex(QGen &realPart, QGen &imaginaryPart) const;
    bool isInteger() const { return expr->is_integer(); }
    bool isBoolean() const { return type() == _INT_ && subtype() == _INT_BOOLEAN; }
    bool isRational() const { return isFraction() && expr->_FRACptr->num.is_integer() && expr->_FRACptr->den.is_integer(); }
    bool isExact() const { return is_exact(*expr); }
    bool isRealConstant() const { return isInteger() || isRational() || isFloatingPointNumber(); }
    bool isModular() const { return type() == _MOD; }
    bool isModular(QGen &value, QGen &modulus) const;
    bool isMatrix() const { return isMatrixVector() || ckmatrix(*expr); }
    bool isSparsePolynomial() const { return type() == _SPOL1; }
    bool isFunction() const { return type() == _FUNC; }
    bool isAlgebraicExtension() const { return type() == _EXT; }
    bool isUserDefined() const { return type() == _USER; }
    bool isUndefined() const { return is_undef(*expr); }
    bool isInfinite() const { return is_inf(*expr); }
    bool isZero() const { return is_zero(*expr, ct); }
    bool isExactlyZero() const { return is_exactly_zero(*expr); }
    bool isVectorOfZeros() const { return isVector() ? is_zero__VECT(*expr->_VECTptr, ct) : false; }
    bool isOne() const { return is_one(*expr); }
    bool isMinusOne() const { return is_minus_one(*expr); }
    bool isEulerMascheroniConstant() const { return isIdentifier() && toString() == "euler_gamma"; }
    bool isEulerNumber() const { return isIdentifier() && toString() == "e"; }
    bool isPi() const { return isIdentifier() && *expr->_IDNTptr == _IDNT_pi(); }
    bool isImaginaryUnit() const { return isIdentifier() && toString() == "i"; }
    bool isNonnegative() const { return is_positive(*expr, ct); }
    bool isNegative() const { return !isNonnegative(); }
    bool isPositive() const { return is_strictly_positive(*expr, ct); }
    bool isRationalExpression() const;
    bool isRationalExpression(QGen &numerator, QGen &denominator) const;
    bool isWellDefined() const { return !has_inf_or_undef(*expr); }
    bool isElementary() const;
    bool isPolynomialRoot() const { return isUnaryFunction(at_rootof); }

    bool containsApproximateValue() const { return expr->is_approx(); }
    bool containsImaginaryUnit() const { return has_i(*expr); }

    // operators
    bool isAssignmentOperator() const { return isUnaryFunction(at_sto); }
    bool isArrayAssignmentOperator() const { return isUnaryFunction(at_array_sto); }
    bool isAtOperator() const { return isUnaryFunction(at_at); }
    bool isSumOperator() const { return isUnaryFunction(at_plus); }
    bool isIncrementOperator() const { return isUnaryFunction(at_increment); }
    bool isDecrementOperator() const { return isUnaryFunction(at_decrement); }
    bool isHadamardSumOperator() const { return isUnaryFunction(at_pointplus); }
    bool isHadamardDifferenceOperator() const { return isUnaryFunction(at_pointminus); }
    bool isProductOperator() const { return isUnaryFunction(at_prod); }
    bool isAmpersandProductOperator() const { return isUnaryFunction(at_ampersand_times); }
    bool isHadamardProductOperator() const { return isUnaryFunction(at_pointprod); }
    bool isFactorialOperator() const { return isUnaryFunction(at_factorial); }
    bool isCrossProductOperator() const { return isUnaryFunction(at_cross); }
    bool isComposeOperator() const { return isUnaryFunction(at_compose); }
    bool isPowerOperator() const { return isUnaryFunction(at_pow); }
    bool isHadamardPowerOperator() const { return isUnaryFunction(at_pointpow); }
    bool isFunctionalPowerOperator() const { return isUnaryFunction(at_composepow); }
    bool isReciprocalOperator() const { return isUnaryFunction(at_inv); }
    bool isHadamardDivisionOperator() const { return isUnaryFunction(at_pointdivision); }
    bool isConjunctionOperator() const { return isUnaryFunction(at_and); }
    bool isDisjunctionOperator() const { return isUnaryFunction(at_ou); }
    bool isExclusiveOrOperator() const { return isUnaryFunction(at_xor); }
    bool isNegationOperator() const { return isUnaryFunction(at_not); }
    bool isBitwiseAndOperator() const { return isUnaryFunction(at_bitand); }
    bool isBitwiseOrOperator() const { return isUnaryFunction(at_bitor); }
    bool isBitwiseXorOperator() const { return isUnaryFunction(at_bitxor); }
    bool isComplexConjugateOperator() const { return isUnaryFunction(at_conj); }
    bool isMinusOperator() const { return isUnaryFunction(at_neg); }
    bool isTranspositionOperator() const { return isUnaryFunction(at_tran); }
    bool isTraceOperator() const { return isUnaryFunction(at_trace); }
    bool isImaginaryPartOperator() const { return isUnaryFunction(at_im); }
    bool isRealPartOperator() const { return isUnaryFunction(at_re); }
    bool isUnionOperator() const { return isUnaryFunction(at_union); }
    bool isIntersectionOperator() const { return isUnaryFunction(at_intersect); }
    bool isSetDifferenceOperator() const { return isUnaryFunction(at_minus); }
    bool isElementOperator() const { return isUnaryFunction(at_contains); }
    bool isEqualOperator() const { return isUnaryFunction(at_same); }
    bool isNotEqualOperator() const { return isUnaryFunction(at_different); }
    bool isLessThanOperator() const { return isUnaryFunction(at_inferieur_strict); }
    bool isGreaterThanOperator() const { return isUnaryFunction(at_superieur_strict); }
    bool isLessThanOrEqualOperator() const { return isUnaryFunction(at_inferieur_egal); }
    bool isGreaterThanOrEqualOperator() const { return isUnaryFunction(at_superieur_egal); }
    bool isConditionalOperator() const { return isUnaryFunction(at_when); }
    bool isIfThenElseOperator() const { return isUnaryFunction(at_ifte); }
    bool isUnitApplicationOperator() const { return isUnaryFunction(at_unit); }
    bool isUnitApplicationOperator(bool &hasConstant) const;
    bool isFunctionApplicationOperator() const { return isUnaryFunction(at_of); }
    bool isFunctionApplicationOperator(QString &functionName, QGen &arguments) const;
    bool isMappingOperator() const { return isUnaryFunction(at_program); }
    bool isMappingOperator(QGen &variables, QGen &expression) const;
    bool isIntervalOperator() const { return isUnaryFunction(at_interval); }
    bool isIntervalOperator(QGen &lowerBound, QGen &upperBound) const;
    bool isDerivativeOperator(bool simple) const;
    bool isSequenceOperator() const { return isUnaryFunction(at_dollar); }
    bool isUserOperator() const { return dynamic_cast<const unary_function_user*>(sommet().ptr()) != 0; }
    bool isUserOperator(QString &name) const;

    int operatorType(int &priority) const;
    int operatorPriority() const;
    bool isOperator(int &type) const;
    bool isOperator() const;
    bool isAssociativeOperator(vecteur &operands) const;
    bool isBinaryOperator(QGen &left, QGen &right) const;
    bool isUnaryOperator(QGen &operand) const;
    bool isTheSameOperatorAs(const QGen &g) const;

    bool isNegativeConstant() const;
    bool isPrecededByMinus() const { return isMinusOperator() || isNegativeConstant(); }
    bool isBinomialCoefficient() const { return isUnaryFunction(at_binomial); }
    bool isBinomialCoefficient(int &n, int &k) const;
    bool isAbsoluteValue() const { return isUnaryFunction(at_abs); }
    bool isEquation() const { return isUnaryFunction(at_equal); }
    bool isEquation(QGen &leftHandSide, QGen &rightHandSide) const;
    bool isInequation() const;
    int isInequation(QGen &leftHandSide, QGen &rightHandSide) const;
    bool isIntegral() const { return isUnaryFunction(at_integrate); }
    bool isIndefiniteIntegral(QGen &expression, QGen &variable) const;
    bool isDefiniteIntegral(QGen &expression, QGen &variable, QGen &lowerBound, QGen &upperBound) const;

    int integerValue() const { return isInteger() ? expr->val : 0; }
    qreal doubleValue() const { return isDouble() ? expr->DOUBLE_val() : 0; }
    qreal approximateValue() const;
    QString stringValue() const { return isString() ? expr->_STRNGptr->data() : ""; }

    int operandCount() const;
    QGen firstOperand() const;
    QGen lastOperand() const;
    QGen secondOperand() const;
    QGen thirdOperand() const;
    QGen nthOperand(int n) const;

    int mapEntriesCount() const { return isMap() ? int(expr->_MAPptr->size()) : -1; }
    QGen valueForKey(const QGen &key) const;
    bool nthMapEntry(int n, QGen &key, QGen &value) const;

    QGen evaluate() const { return QGen(_eval(*expr, ct), ct); }
    QGen simplify() const { return QGen(_simplify(*expr, ct), ct); }

    int length() const { return isVector() ? expr->_VECTptr->size() : -1; }
    int rows() const { return isMatrix() ? _dim(*expr, ct)._VECTptr->front().val : -1; }
    int columns() const { return isMatrix() ? _dim(*expr, ct)._VECTptr->back().val : -1; }
    QGen &toSequenceVector() { if (isVector()) expr->operator =(expr->change_subtype(_SEQ__VECT)); return *this; }
    QGen &toSetVector() { if (isVector()) expr->operator =(expr->change_subtype(_SET__VECT)); return *this; }
    QGen &toListVector() { if (isVector()) expr->operator =(expr->change_subtype(_LIST__VECT)); return *this; }
    QGen &toPolynomVector() { if (isVector()) expr->operator =(expr->change_subtype(_POLY__VECT)); return *this; }
    bool resizeVector(int n);
    QGen vectorPopFront();

    QPicture render(int alignment = AlignLeft | AlignBaseline);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGen::Alignment)

#endif // QGEN_H
