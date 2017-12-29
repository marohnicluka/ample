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
#include <QVector>
#include <QString>
#include <QStringList>

class MathGlyphs
{
    static QStringList smallGreekLetterNames;
    static QStringList capitalGreekLetterNames;

public:
    enum LetterType { Serif, Sans, Mono, Script, Fraktur, DoubleStruck, Greek };

    static QString encodeUcs4(uint ucs4);
    static bool getGreekLetter(const QString &name, QChar &letter);
    static QString letterToMath(QChar letter, LetterType letterType, bool bold, bool italic);
    static QString digitsToSuperscript(const QString &digits);
    static QString digitsToSubscript(const QString &digits);

    // spaces
    static QChar functionApplicationSpace()             { return QChar(0x2061); }
    static QChar invisibleTimesSpace()                  { return QChar(0x2062); }
    static QChar invisibleSeparatorSpace()              { return QChar(0x2063); }
    static QChar enQuadSpace()                          { return QChar(0x2000); }
    static QChar emQuadSpace()                          { return QChar(0x2001); }
    static QChar zeroWidthSpace()                       { return QChar(0x200B); }
    static QChar hairSpace()                            { return QChar(0x200A); }
    static QChar thinSpace()                            { return QChar(0x2009); }
    static QChar mediumSpace()                          { return QChar(0x205f); }
    static QChar thickSpace()                           { return QChar(0x2004); }
    static QChar digitWidthSpace()                      { return QChar(0x2007); }
    static QChar noBreakSpace()                         { return QChar(0x00A0); }

    // operators
    static QChar complementSymbol()                     { return QChar(0x2201); }
    static QChar partialDifferentialSymbol()            { return QChar(0x2202); }
    static QChar incrementSymbol()                      { return QChar(0x2206); }
    static QChar nablaSymbol()                          { return QChar(0x2207); }
    static QChar elementOfSymbol()                      { return QChar(0x2208); }
    static QChar nAryProductSymbol()                    { return QChar(0x220f); }
    static QChar nArySummationSymbol()                  { return QChar(0x2211); }
    static QChar minusSignSymbol()                      { return QChar(0x2212); }
    static QChar superscriptMinusSignSymbol()           { return QChar(0x207b); }
    static QChar subscriptMinusSignSymbol()             { return QChar(0x208b); }
    static QChar divisionSlashSymbol()                  { return QChar(0x2215); }
    static QChar setMinusSymbol()                       { return QChar(0x2216); }
    static QChar asteriskOperatorSymbol()               { return QChar(0x2217); }
    static QChar notSignSymbol()                        { return QChar(0x00ac); }
    static QChar notEqualToSymbol()                     { return QChar(0x2260); }
    static QChar multiplicationSignSymbol()             { return QChar(0x00d7); }
    static QChar multiplicationDotSymbol()              { return QChar(0x22c5); }
    static QChar ringOperatorSymbol()                   { return QChar(0x2218); }
    static QChar squareRootSymbol()                     { return QChar(0x221a); }
    static QChar cubeRootSymbol()                       { return QChar(0x221b); }
    static QChar fourthRootSymbol()                     { return QChar(0x221c); }
    static QChar logicalAndSymbol()                     { return QChar(0x2227); }
    static QChar logicalOrSymbol()                      { return QChar(0x2228); }
    static QChar logicalXorSymbol()                     { return QChar(0x22BB); }
    static QChar intersectionSymbol()                   { return QChar(0x2229); }
    static QChar unionSymbol()                          { return QChar(0x222a); }
    static QChar lessThanOrEqualToSymbol()              { return QChar(0x2264); }
    static QChar greaterThanOrEqualToSymbol()           { return QChar(0x2265); }
    static QChar lessThanOrEqualToSlantedSymbol()       { return QChar(0x2a7d); }
    static QChar greaterThanOrEqualToSlantedSymbol()    { return QChar(0x2a7e); }
    static QChar doubleFactorialSymbol()                { return QChar(0x203c); }
    static QChar plusMinusSymbol()                      { return QChar(0x00b1); }
    static QChar minusPlusSymbol()                      { return QChar(0x2213); }
    static QChar colonEqualsSymbol()                    { return QChar(0x2254); }
    static QChar equalToByDefinitionSymbol()            { return QChar(0x225d); }
    static QChar questionedEqualToSymbol()              { return QChar(0x225f); }
    static QChar primeSymbol()                          { return QChar(0x2032); }
    static QChar doublePrimeSymbol()                    { return QChar(0x2033); }
    static QChar triplePrimeSymbol()                    { return QChar(0x2034); }
    static QChar ratioSymbol()                          { return QChar(0x2236); }
    static QChar circledDivisionSlashSymbol()           { return QChar(0x2298); }

    // special symbols
    static QChar emptySetSymbol()                       { return QChar(0x2205); }
    static QChar infinitySymbol()                       { return QChar(0x221e); }
    static QChar midlineHorizontalEllipsis()            { return QChar(0x22ef); }
    static QChar twoDotLeader()                         { return QChar(0x2025); }
    static QChar leftwardsArrow()                       { return QChar(0x2190); }
    static QChar rightwardsArrow()                      { return QChar(0x2192); }
    static QChar longLeftwardsArrow()                   { return QChar(0x27f5); }
    static QChar longRightwardsArrow()                  { return QChar(0x27f6); }
    static QChar rightwardsArrowFromBar()               { return QChar(0x21a6); }
    static QChar placeholderSymbol()                    { return QChar(0x25a3); }
    static QChar leftDoubleQuotationMark()              { return QChar(0x201c); }
    static QChar rightDoubleQuotationMark()             { return QChar(0x201d); }
    static QChar alephSymbol()                          { return QChar(0x2135); }
    static QChar dotsSymbol()                           { return QChar(0x2026); }
    static QChar diagonalDotsSymbol()                   { return QChar(0x22f1); }
    static QChar rightDiagonalDotsSymbol()              { return QChar(0x22f0); }
    static QChar verticalDotsSymbol()                   { return QChar(0x22ee); }
    static QChar centeredDotsSymbol()                   { return QChar(0x22ef); }
    static QChar iMathSymbol()                          { return QChar(0x0131); }
    static QChar jMathSymbol()                          { return QChar(0x0237); }
    static QChar degreeSymbol()                         { return QChar(0x00b0); }
    static QChar degreeCelsiusSymbol()                  { return QChar(0x2103); }
    static QChar degreeFahrenheitSymbol()               { return QChar(0x2109); }

    // letters
    static QChar smallLetterHWithStroke()               { return QChar(0x0127); }
    static QChar smallLetterPi()                        { return QChar(0x03c0); }
    static QChar smallLetterEpsilon()                   { return QChar(0x03b5); }
    static QChar subscriptSmallLetterA()                { return QChar(0x2090); }
    static QChar subscriptSmallLetterE()                { return QChar(0x2091); }
    static QChar subscriptSmallLetterO()                { return QChar(0x2092); }
    static QChar subscriptSmallLetterX()                { return QChar(0x2093); }
    static QChar subscriptSmallLetterH()                { return QChar(0x2095); }
    static QChar subscriptSmallLetterK()                { return QChar(0x2096); }
    static QChar subscriptSmallLetterL()                { return QChar(0x2097); }
    static QChar subscriptSmallLetterM()                { return QChar(0x2098); }
    static QChar subscriptSmallLetterN()                { return QChar(0x2099); }
    static QChar subscriptSmallLetterP()                { return QChar(0x209a); }
    static QChar subscriptSmallLetterS()                { return QChar(0x209b); }
    static QChar subscriptSmallLetterT()                { return QChar(0x209c); }

    // accents
    static QChar hatAccentSymbol()                      { return QChar(0x0302); }
    static QChar checkAccentSymbol()                    { return QChar(0x030c); }
    static QChar tildeAccentSymbol()                    { return QChar(0x0303); }
    static QChar acuteAccentSymbol()                    { return QChar(0x0301); }
    static QChar graveAccentSymbol()                    { return QChar(0x0300); }
    static QChar dotAccentSymbol()                      { return QChar(0x0307); }
    static QChar doubleDotAccentSymbol()                { return QChar(0x0308); }
    static QChar tripleDotAccentSymbol()                { return QChar(0x20db); }
    static QChar barAccentSymbol()                      { return QChar(0x0304); }
    static QChar vecAccentSymbol()                      { return QChar(0x20d7); }

    // brackets
    static QChar leftCeilingBracket()                   { return QChar(0x2308); }
    static QChar rightCeilingBracket()                  { return QChar(0x2309); }
    static QChar leftFloorBracket()                     { return QChar(0x230a); }
    static QChar rightFloorBracket()                    { return QChar(0x230b); }
    static QChar topHalfIntegral()                      { return QChar(0x2320); }
    static QChar bottomHalfIntegral()                   { return QChar(0x2321); }
    static QChar leftParenthesisUpperHook()             { return QChar(0x239b); }
    static QChar leftParenthesisExtension()             { return QChar(0x239c); }
    static QChar leftParenthesisLowerHook()             { return QChar(0x239d); }
    static QChar rightParenthesisUpperHook()            { return QChar(0x239e); }
    static QChar rightParenthesisExtension()            { return QChar(0x239f); }
    static QChar rightParenthesisLowerHook()            { return QChar(0x23A0); }
    static QChar leftSquareBracketUpperCorner()         { return QChar(0x239b); }
    static QChar leftSquareBracketExtension()           { return QChar(0x239c); }
    static QChar leftSquareBracketLowerCorner()         { return QChar(0x239d); }
    static QChar rightSquareBracketUpperCorner()        { return QChar(0x239e); }
    static QChar rightSquareBracketExtension()          { return QChar(0x239f); }
    static QChar rightSquareBracketLowerCorner()        { return QChar(0x23A0); }
    static QChar leftCurlyBracketUpperHook()            { return QChar(0x23a7); }
    static QChar leftCurlyBracketMiddlePiece()          { return QChar(0x23a8); }
    static QChar leftCurlyBracketLowerHook()            { return QChar(0x23a9); }
    static QChar leftCurlyBracketExtension()            { return QChar(0x23aa); }
    static QChar rightCurlyBracketUpperHook()           { return QChar(0x23ab); }
    static QChar rightCurlyBracketMiddlePiece()         { return QChar(0x23ac); }
    static QChar rightCurlyBracketLowerHook()           { return QChar(0x23ad); }
    static QChar rightCurlyBracketExtension()           { return QChar(0x23ae); }
    static QChar horizontalLineExtension()              { return QChar(0x23af); }
    static QChar verticalLineExtension()                { return QChar(0x23d0); }
    static QChar upperLeftCurlyBracketSection()         { return QChar(0x23b0); }
    static QChar lowerLeftCurlyBracketSection()         { return QChar(0x23b1); }
    static QChar radicalSymbolBottom()                  { return QChar(0x23b7); }
    static QChar leftWhiteSquareBracket()               { return QChar(0x27e6); }
    static QChar rightWhiteSquareBracket()              { return QChar(0x27e7); }
    static QChar leftAngleBracket()                     { return QChar(0x27e8); }
    static QChar rightAngleBracket()                    { return QChar(0x27e9); }
    static QChar leftWhiteCurlyBracket()                { return QChar(0x2983); }
    static QChar rightWhiteCurlyBracket()               { return QChar(0x2984); }
    static QChar superscriptLeftParenthesis()           { return QChar(0x207d); }
    static QChar superscriptRightParenthesis()          { return QChar(0x207e); }
    static QChar subscriptLeftParenthesis()             { return QChar(0x208d); }
    static QChar subscriptRightParenthesis()            { return QChar(0x208e); }
    static QChar absoluteValueBracket()                 { return QChar(0x007c); }
    static QChar doubleAbsoluteValueBracket()           { return QChar(0x2016); }
};

#endif // GLYPHS_H
