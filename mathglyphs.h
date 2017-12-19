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

#ifndef GLYPHS_H
#define GLYPHS_H

#include <QChar>
#include <QFont>
#include <QFontMetricsF>
#include <QStringList>

class MathGlyphs : public QFontMetricsF
{
    QStringList smallGreekLetterNames;
    QStringList capitalGreekLetterNames;

public:
    enum LetterType { Serif, Sans, Mono, Script, Fraktur, DoubleStruck, Greek };
    MathGlyphs(const QFont &font);
    bool isGreekLetter(const QString &name, QChar &chr);

    static QChar letterToMath(QChar letter, LetterType letterType, bool bold, bool italic);
    static QString digitsToSuperscript(const QString &digits);
    static QString digitsToSubscript(const QString &digits);

    // spaces
    static QChar functionApplicationSpace()              { return QChar(0x2061); }
    static QChar invisibleTimesSpace()                   { return QChar(0x2062); }
    static QChar invisibleSeparatorSpace()               { return QChar(0x2063); }
    static QChar enQuadSpace()                           { return QChar(0x2000); }
    static QChar emQuadSpace()                           { return QChar(0x2001); }
    static QChar zeroWidthMathSpace()                    { return QChar(0x200B); }
    static QChar veryVeryThinMathSpace()                 { return QChar(0x200A); }
    static QChar thinMathSpace()                         { return QChar(0x2009); }
    static QChar mediumMathSpace()                       { return QChar(0x205f); }
    static QChar thickMathSpace()                        { return QChar(0x2005); }
    static QChar veryThickMathSpace()                    { return QChar(0x2004); }
    static QChar digitWidthSpace()                       { return QChar(0x2007); }
    static QChar noBreakSpace()                          { return QChar(0x00A0); }

    // operators
    static QChar complementSymbol()                      { return QChar(0x2201); }
    static QChar partialDifferentialSymbol()             { return QChar(0x2202); }
    static QChar incrementSymbol()                       { return QChar(0x2206); }
    static QChar nablaSymbol()                           { return QChar(0x2207); }
    static QChar elementOfSymbol()                       { return QChar(0x2208); }
    static QChar nAryProductSymbol()                     { return QChar(0x220f); }
    static QChar nArySummationSymbol()                   { return QChar(0x2211); }
    static QChar minusSignSymbol()                       { return QChar(0x2212); }
    static QChar superscriptMinusSignSymbol()            { return QChar(0x207b); }
    static QChar subscriptMinusSignSymbol()              { return QChar(0x208b); }
    static QChar divisionSlashSymbol()                   { return QChar(0x2215); }
    static QChar setMinusSymbol()                        { return QChar(0x2216); }
    static QChar asteriskOperatorSymbol()                { return QChar(0x2217); }
    static QChar notSignSymbol()                         { return QChar(0x00ac); }
    static QChar notEqualToSymbol()                      { return QChar(0x2260); }
    static QChar multiplicationSignSymbol()              { return QChar(0x00d7); }
    static QChar multiplicationDotSymbol()               { return QChar(0x22c5); }
    static QChar ringOperatorSymbol()                    { return QChar(0x2218); }
    static QChar squareRootSymbol()                      { return QChar(0x221a); }
    static QChar logicalAndSymbol()                      { return QChar(0x2227); }
    static QChar logicalOrSymbol()                       { return QChar(0x2228); }
    static QChar intersectionSymbol()                    { return QChar(0x2229); }
    static QChar unionSymbol()                           { return QChar(0x222a); }
    static QChar lessThanOrEqualToSymbol()               { return QChar(0x2264); }
    static QChar greaterThanOrEqualToSymbol()            { return QChar(0x2265); }
    static QChar lessThanOrEqualToSlantedSymbol()        { return QChar(0x2a7d); }
    static QChar greaterThanOrEqualToSlantedSymbol()     { return QChar(0x2a7e); }
    static QChar doubleFactorialSymbol()                 { return QChar(0x203c); }
    static QChar plusMinusSymbol()                       { return QChar(0x00b1); }
    static QChar minusPlusSymbol()                       { return QChar(0x2213); }
    static QChar definitionEqualitySymbol()              { return QChar(0x2254); }
    static QChar primeSymbol()                           { return QChar(0x2032); }
    static QChar doublePrimeSymbol()                     { return QChar(0x2033); }
    static QChar triplePrimeSymbol()                     { return QChar(0x2034); }
    static QChar ratioSymbol()                           { return QChar(0x2236); }

    // special symbols
    static QChar emptySetSymbol()                        { return QChar(0x2205); }
    static QChar infinitySymbol()                        { return QChar(0x221e); }
    static QChar midlineHorizontalEllipsisSymbol()       { return QChar(0x22ef); }
    static QChar twoDotLeaderSymbol()                    { return QChar(0x2025); }
    static QChar leftwardsArrowSymbol()                  { return QChar(0x2190); }
    static QChar rightwardsArrowSymbol()                 { return QChar(0x2192); }
    static QChar longLeftwardsArrowSymbol()              { return QChar(0x27f5); }
    static QChar longRightwardsArrowSymbol()             { return QChar(0x27f6); }
    static QChar rightwardsArrowFromBarSymbol()          { return QChar(0x21a6); }
    static QChar placeholderSymbol()                     { return QChar(0x25a3); }
    static QChar leftDoubleQuotationMarkSymbol()         { return QChar(0x201c); }
    static QChar rightDoubleQuotationMarkSymbol()        { return QChar(0x201d); }
    static QChar smallGreekGammaSymbol()                 { return QChar(0x03b3); }
    static QChar alephSymbol()                           { return QChar(0x2135); }
    static QChar dotsSymbol()                            { return QChar(0x2026); }
    static QChar diagonalDotsSymbol()                    { return QChar(0x22f1); }
    static QChar rightDiagonalDotsSymbol()               { return QChar(0x22f0); }
    static QChar verticalDotsSymbol()                    { return QChar(0x22ee); }
    static QChar centeredDotsSymbol()                    { return QChar(0x22ef); }
    static QChar iMathSymbol()                           { return QChar(0x0131); }
    static QChar jMathSymbol()                           { return QChar(0x0237); }

    // accents
    static QChar hatAccentSymbol()                       { return QChar(0x0302); }
    static QChar checkAccentSymbol()                     { return QChar(0x030c); }
    static QChar tildeAccentSymbol()                     { return QChar(0x0303); }
    static QChar acuteAccentSymbol()                     { return QChar(0x0301); }
    static QChar graveAccentSymbol()                     { return QChar(0x0300); }
    static QChar dotAccentSymbol()                       { return QChar(0x0307); }
    static QChar doubleDotAccentSymbol()                 { return QChar(0x0308); }
    static QChar tripleDotAccentSymbol()                 { return QChar(0x20db); }
    static QChar barAccentSymbol()                       { return QChar(0x0304); }
    static QChar vecAccentSymbol()                       { return QChar(0x20d7); }

    // brackets
    static QChar leftCeilingSymbol()                     { return QChar(0x2308); }
    static QChar rightCeilingSymbol()                    { return QChar(0x2309); }
    static QChar leftFloorSymbol()                       { return QChar(0x230a); }
    static QChar rightFloorSymbol()                      { return QChar(0x230b); }
    static QChar topHalfIntegralSymbol()                 { return QChar(0x2320); }
    static QChar bottomHalfIntegralSymbol()              { return QChar(0x2321); }
    static QChar leftParenthesisUpperHookSymbol()        { return QChar(0x239b); }
    static QChar leftParenthesisExtensionSymbol()        { return QChar(0x239c); }
    static QChar leftParenthesisLowerHookSymbol()        { return QChar(0x239d); }
    static QChar rightParenthesisUpperHookSymbol()       { return QChar(0x239e); }
    static QChar rightParenthesisExtensionSymbol()       { return QChar(0x239f); }
    static QChar rightParenthesisLowerHookSymbol()       { return QChar(0x23A0); }
    static QChar leftSquareBracketUpperCornerSymbol()    { return QChar(0x239b); }
    static QChar leftSquareBracketExtensionSymbol()      { return QChar(0x239c); }
    static QChar leftSquareBracketLowerCornerSymbol()    { return QChar(0x239d); }
    static QChar rightSquareBracketUpperCornerSymbol()   { return QChar(0x239e); }
    static QChar rightSquareBracketExtensionSymbol()     { return QChar(0x239f); }
    static QChar rightSquareBracketLowerCornerSymbol()   { return QChar(0x23A0); }
    static QChar leftCurlyBracketUpperHookSymbol()       { return QChar(0x23a7); }
    static QChar leftCurlyBracketMiddlePieceSymbol()     { return QChar(0x23a8); }
    static QChar leftCurlyBracketLowerHookSymbol()       { return QChar(0x23a9); }
    static QChar leftCurlyBracketExtensionSymbol()       { return QChar(0x23aa); }
    static QChar rightCurlyBracketUpperHookSymbol()      { return QChar(0x23ab); }
    static QChar rightCurlyBracketMiddlePieceSymbol()    { return QChar(0x23ac); }
    static QChar rightCurlyBracketLowerHookSymbol()      { return QChar(0x23ad); }
    static QChar rightCurlyBracketExtensionSymbol()      { return QChar(0x23ae); }
    static QChar horizontalLineExtensionSymbol()         { return QChar(0x23af); }
    static QChar verticalLineExtensionSymbol()           { return QChar(0x23d0); }
    static QChar upperLeftCurlyBracketSectionSymbol()    { return QChar(0x23b0); }
    static QChar lowerLeftCurlyBracketSectionSymbol()    { return QChar(0x23b1); }
    static QChar radicalSymbolBottomSymbol()             { return QChar(0x23b7); }
    static QChar leftWhiteSquareBracketSymbol()          { return QChar(0x27e6); }
    static QChar rightWhiteSquareBracketSymbol()         { return QChar(0x27e7); }
    static QChar leftAngleBracketSymbol()                { return QChar(0x27e8); }
    static QChar rightAngleBracketSymbol()               { return QChar(0x27e9); }
    static QChar leftWhiteCurlyBracketSymbol()           { return QChar(0x2983); }
    static QChar rightWhiteCurlyBracketSymbol()          { return QChar(0x2984); }
    static QChar superscriptLeftParenthesisSymbol()      { return QChar(0x207d); }
    static QChar superscriptRightParenthesisSymbol()     { return QChar(0x207e); }
    static QChar subscriptLeftParenthesisSymbol()        { return QChar(0x208d); }
    static QChar subscriptRightParenthesisSymbol()       { return QChar(0x208e); }
    static QChar absoluteValueBracketSymbol()            { return QChar(0x007c); }
    static QChar doubleAbsoluteValueBracketSymbol()      { return QChar(0x2016); }
};

#endif // GLYPHS_H
