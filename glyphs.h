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

class Glyphs : public QFontMetricsF
{
    QStringList smallGreekLetterNames;
    QStringList capitalGreekLetterNames;

public:
    enum LetterType { Serif, Sans, Mono, Script, Fraktur, DoubleStruck, Greek };
    Glyphs(const QFont &font);
    bool isGreekLetter(const QString &name, QChar &chr);

    static QChar letterToMath(QChar letter, LetterType letterType, bool bold, bool italic);
    static QString digitsToSuperscript(const QString &digits);
    static QString digitsToSubscript(const QString &digits);

    // spaces
    static inline QChar functionApplicationSpace()              { return QChar(0x2061); }
    static inline QChar invisibleTimesSpace()                   { return QChar(0x2062); }
    static inline QChar invisibleSeparatorSpace()               { return QChar(0x2063); }
    static inline QChar enQuadSpace()                           { return QChar(0x2000); }
    static inline QChar emQuadSpace()                           { return QChar(0x2001); }
    static inline QChar mediumMathematicalSpace()               { return QChar(0x205f); }

    // operators
    static inline QChar complementSymbol()                      { return QChar(0x2201); }
    static inline QChar partialDifferentialSymbol()             { return QChar(0x2202); }
    static inline QChar incrementSymbol()                       { return QChar(0x2206); }
    static inline QChar nablaSymbol()                           { return QChar(0x2207); }
    static inline QChar elementOfSymbol()                       { return QChar(0x2208); }
    static inline QChar nAryProductSymbol()                     { return QChar(0x220f); }
    static inline QChar nArySummationSymbol()                   { return QChar(0x2211); }
    static inline QChar minusSignSymbol()                       { return QChar(0x2212); }
    static inline QChar superscriptMinusSignSymbol()            { return QChar(0x207b); }
    static inline QChar subscriptMinusSignSymbol()              { return QChar(0x208b); }
    static inline QChar divisionSlashSymbol()                   { return QChar(0x2215); }
    static inline QChar setMinusSymbol()                        { return QChar(0x2216); }
    static inline QChar asteriskOperatorSymbol()                { return QChar(0x2217); }
    static inline QChar notSignSymbol()                         { return QChar(0x00ac); }
    static inline QChar notEqualToSymbol()                      { return QChar(0x2260); }
    static inline QChar multiplicationSignSymbol()              { return QChar(0x00d7); }
    static inline QChar ringOperatorSymbol()                    { return QChar(0x2218); }
    static inline QChar squareRootSymbol()                      { return QChar(0x221a); }
    static inline QChar logicalAndSymbol()                      { return QChar(0x2227); }
    static inline QChar logicalOrSymbol()                       { return QChar(0x2228); }
    static inline QChar intersectionSymbol()                    { return QChar(0x2229); }
    static inline QChar unionSymbol()                           { return QChar(0x222a); }
    static inline QChar lessThanOrEqualToSymbol()               { return QChar(0x2264); }
    static inline QChar greaterThanOrEqualToSymbol()            { return QChar(0x2265); }
    static inline QChar lessThanOrEqualToSlantedSymbol()        { return QChar(0x2a7d); }
    static inline QChar greaterThanOrEqualToSlantedSymbol()     { return QChar(0x2a7e); }

    // special symbols
    static inline QChar emptySetSymbol()                        { return QChar(0x2205); }
    static inline QChar infinitySymbol()                        { return QChar(0x221e); }
    static inline QChar midlineHorizontalEllipsisSymbol()       { return QChar(0x22ef); }
    static inline QChar twoDotLeaderSymbol()                    { return QChar(0x2025); }
    static inline QChar leftwardsArrowSymbol()                  { return QChar(0x2190); }
    static inline QChar rightwardsArrowSymbol()                 { return QChar(0x2192); }
    static inline QChar longLeftwardsArrowSymbol()              { return QChar(0x27f5); }
    static inline QChar longRightwardsArrowSymbol()             { return QChar(0x27f6); }
    static inline QChar rightwardsArrowFromBarSymbol()          { return QChar(0x21a6); }
    static inline QChar placeholderSymbol()                     { return QChar(0x25a3); }
    static inline QChar leftDoubleQuotationMarkSymbol()         { return QChar(0x201c); }
    static inline QChar rightDoubleQuotationMarkSymbol()        { return QChar(0x201d); }
    static inline QChar smallGreekGammaSymbol()                 { return QChar(0x03b3); }

    // brackets and composite characters
    static inline QChar leftCeilingSymbol()                     { return QChar(0x2308); }
    static inline QChar rightCeilingSymbol()                    { return QChar(0x2309); }
    static inline QChar leftFloorSymbol()                       { return QChar(0x230a); }
    static inline QChar rightFloorSymbol()                      { return QChar(0x230b); }
    static inline QChar topHalfIntegralSymbol()                 { return QChar(0x2320); }
    static inline QChar bottomHalfIntegralSymbol()              { return QChar(0x2321); }
    static inline QChar leftParenthesisUpperHookSymbol()        { return QChar(0x239b); }
    static inline QChar leftParenthesisExtensionSymbol()        { return QChar(0x239c); }
    static inline QChar leftParenthesisLowerHookSymbol()        { return QChar(0x239d); }
    static inline QChar rightParenthesisUpperHookSymbol()       { return QChar(0x239e); }
    static inline QChar rightParenthesisExtensionSymbol()       { return QChar(0x239f); }
    static inline QChar rightParenthesisLowerHookSymbol()       { return QChar(0x23A0); }
    static inline QChar leftSquareBracketUpperCornerSymbol()    { return QChar(0x239b); }
    static inline QChar leftSquareBracketExtensionSymbol()      { return QChar(0x239c); }
    static inline QChar leftSquareBracketLowerCornerSymbol()    { return QChar(0x239d); }
    static inline QChar rightSquareBracketUpperCornerSymbol()   { return QChar(0x239e); }
    static inline QChar rightSquareBracketExtensionSymbol()     { return QChar(0x239f); }
    static inline QChar rightSquareBracketLowerCornerSymbol()   { return QChar(0x23A0); }
    static inline QChar leftCurlyBracketUpperHookSymbol()       { return QChar(0x23a7); }
    static inline QChar leftCurlyBracketMiddlePieceSymbol()     { return QChar(0x23a8); }
    static inline QChar leftCurlyBracketLowerHookSymbol()       { return QChar(0x23a9); }
    static inline QChar leftCurlyBracketExtensionSymbol()       { return QChar(0x23aa); }
    static inline QChar rightCurlyBracketUpperHookSymbol()      { return QChar(0x23ab); }
    static inline QChar rightCurlyBracketMiddlePieceSymbol()    { return QChar(0x23ac); }
    static inline QChar rightCurlyBracketLowerHookSymbol()      { return QChar(0x23ad); }
    static inline QChar rightCurlyBracketExtensionSymbol()      { return QChar(0x23ae); }
    static inline QChar horizontalLineExtensionSymbol()         { return QChar(0x23af); }
    static inline QChar verticalLineExtensionSymbol()           { return QChar(0x23d0); }
    static inline QChar upperLeftCurlyBracketSectionSymbol()    { return QChar(0x23b0); }
    static inline QChar lowerLeftCurlyBracketSectionSymbol()    { return QChar(0x23b1); }
    static inline QChar radicalSymbolBottomSymbol()             { return QChar(0x23b7); }
    static inline QChar leftWhiteSquareBracketSymbol()          { return QChar(0x27e6); }
    static inline QChar rightWhiteSquareBracketSymbol()         { return QChar(0x27e7); }
    static inline QChar leftAngleBracketSymbol()                { return QChar(0x27e8); }
    static inline QChar rightAngleBracketSymbol()               { return QChar(0x27e9); }
    static inline QChar leftWhiteCurlyBracketSymbol()           { return QChar(0x2983); }
    static inline QChar rightWhiteCurlyBracketSymbol()          { return QChar(0x2984); }
    static inline QChar superscriptLeftParenthesisSymbol()      { return QChar(0x207d); }
    static inline QChar superscriptRightParenthesisSymbol()     { return QChar(0x207e); }
    static inline QChar subscriptLeftParenthesisSymbol()        { return QChar(0x208d); }
    static inline QChar subscriptRightParenthesisSymbol()       { return QChar(0x208e); }
};

#endif // GLYPHS_H
