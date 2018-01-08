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
    static QChar functionApplicationSpace()         { return QChar(0x2061); }
    static QChar invisibleTimesSpace()              { return QChar(0x2062); }
    static QChar invisibleSeparatorSpace()          { return QChar(0x2063); }
    static QChar enQuadSpace()                      { return QChar(0x2000); }
    static QChar emQuadSpace()                      { return QChar(0x2001); }
    static QChar zeroWidthSpace()                   { return QChar(0x200B); }
    static QChar hairSpace()                        { return QChar(0x200A); }
    static QChar thinSpace()                        { return QChar(0x2009); }
    static QChar mediumSpace()                      { return QChar(0x205f); }
    static QChar thickSpace()                       { return QChar(0x2004); }
    static QChar digitWidthSpace()                  { return QChar(0x2007); }
    static QChar noBreakSpace()                     { return QChar(0x00A0); }

    // operators
    static QChar complement()                               { return QChar(0x2201); }
    static QChar partialDifferential()                      { return QChar(0x2202); }
    static QChar increment()                                { return QChar(0x2206); }
    static QChar nabla()                                    { return QChar(0x2207); }
    static QChar nAryProduct()                              { return QChar(0x220f); }
    static QChar nArySummation()                            { return QChar(0x2211); }
    static QChar minus()                                    { return QChar(0x2212); }
    static QChar superscriptMinus()                         { return QChar(0x207b); }
    static QChar subscriptMinus()                           { return QChar(0x208b); }
    static QChar divisionSlash()                            { return QChar(0x2215); }
    static QChar setMinus()                                 { return QChar(0x2216); }
    static QChar asteriskOperator()                         { return QChar(0x2217); }
    static QChar notSign()                                  { return QChar(0x00ac); }
    static QChar notEqualTo()                               { return QChar(0x2260); }
    static QChar times()                                    { return QChar(0x00d7); }
    static QChar multiplicationDot()                        { return QChar(0x22c5); }
    static QChar ringOperator()                             { return QChar(0x2218); }
    static QChar squareRoot()                               { return QChar(0x221a); }
    static QChar cubeRoot()                                 { return QChar(0x221b); }
    static QChar fourthRoot()                               { return QChar(0x221c); }
    static QChar logicalAnd()                               { return QChar(0x2227); }
    static QChar logicalOr()                                { return QChar(0x2228); }
    static QChar logicalXor()                               { return QChar(0x22BB); }
    static QChar setIntersection()                          { return QChar(0x2229); }
    static QChar setUnion()                                 { return QChar(0x222a); }
    static QChar lessThanOrEqualTo()                        { return QChar(0x2264); }
    static QChar greaterThanOrEqualTo()                     { return QChar(0x2265); }
    static QChar lessThanOrEqualToSlanted()                 { return QChar(0x2a7d); }
    static QChar greaterThanOrEqualToSlanted()              { return QChar(0x2a7e); }
    static QChar doubleFactorial()                          { return QChar(0x203c); }
    static QChar plusMinus()                                { return QChar(0x00b1); }
    static QChar minusPlus()                                { return QChar(0x2213); }
    static QChar equalToByDefinition()                      { return QChar(0x225d); }
    static QChar questionedEqualTo()                        { return QChar(0x225f); }
    static QChar prime()                                    { return QChar(0x2032); }
    static QChar doublePrime()                              { return QChar(0x2033); }
    static QChar triplePrime()                              { return QChar(0x2034); }
    static QChar ratio()                                    { return QChar(0x2236); }
    static QChar divides()                                  { return QChar(0x2223); }
    static QChar doesNotDivide()                            { return QChar(0x2224); }
    static QChar parallelTo()                               { return QChar(0x2225); }
    static QChar notParallelTo()                            { return QChar(0x2226); }
    static QChar dotMinus()                                 { return QChar(0x2238); }
    static QChar excess()                                   { return QChar(0x2239); }
    static QChar geometricProportion()                      { return QChar(0x223a); }
    static QChar homothetic()                               { return QChar(0x223b); }
    static QChar tildeOperator()                            { return QChar(0x223c); }
    static QChar reversedTilde()                            { return QChar(0x223d); }
    static QChar wreathProduct()                            { return QChar(0x2240); }
    static QChar notTilde()                                 { return QChar(0x2241); }
    static QChar minusTilde()                               { return QChar(0x2242); }
    static QChar asymptoticallyEqualTo()                    { return QChar(0x2243); }
    static QChar notAsymptoticallyEqualTo()                 { return QChar(0x2244); }
    static QChar approximatelyEqualTo()                     { return QChar(0x2245); }
    static QChar approximatelyButNotActuallyEqualTo()       { return QChar(0x2246); }
    static QChar notEqualOrApproximatelyEqualTo()           { return QChar(0x2247); }
    static QChar almostEqualTo()                            { return QChar(0x2248); }
    static QChar notAlmostEqualTo()                         { return QChar(0x2249); }
    static QChar almostEqualOrEqualTo()                     { return QChar(0x224a); }
    static QChar tripleTilde()                              { return QChar(0x224b); }
    static QChar allEqualTo()                               { return QChar(0x224c); }
    static QChar equivalentTo()                             { return QChar(0x224d); }
    static QChar geometricallyEquivalentTo()                { return QChar(0x224e); }
    static QChar differenceBetween()                        { return QChar(0x224f); }
    static QChar approachesTheLimit()                       { return QChar(0x2250); }
    static QChar geometricallyEqualTo()                     { return QChar(0x2251); }
    static QChar approximatelyEqualToOrImageOf()            { return QChar(0x2252); }
    static QChar imageOfOrApproximatelyEqualTo()            { return QChar(0x2253); }
    static QChar colonEquals()                              { return QChar(0x2254); }
    static QChar equalsColon()                              { return QChar(0x2255); }
    static QChar ringInEqualTo()                            { return QChar(0x2256); }
    static QChar ringEqualTo()                              { return QChar(0x2257); }
    static QChar correspondsTo()                            { return QChar(0x2258); }
    static QChar estimates()                                { return QChar(0x2259); }
    static QChar equiangularTo()                            { return QChar(0x225a); }
    static QChar starEquals()                               { return QChar(0x225b); }
    static QChar deltaEqualTo()                             { return QChar(0x225c); }
    static QChar measuredBy()                               { return QChar(0x225e); }
    static QChar identicalTo()                              { return QChar(0x2261); }
    static QChar notIdenticalTo()                           { return QChar(0x2262); }
    static QChar strictlyEquivalentTo()                     { return QChar(0x2263); }
    static QChar lessThanOverEqualTo()                      { return QChar(0x2266); }
    static QChar greaterThanOverEqualTo()                   { return QChar(0x2267); }
    static QChar lessThanButNotEqualTo()                    { return QChar(0x2268); }
    static QChar greaterThanButNotEqualTo()                 { return QChar(0x2269); }
    static QChar muchLessThan()                             { return QChar(0x226a); }
    static QChar muchGreaterThan()                          { return QChar(0x226b); }
    static QChar between()                                  { return QChar(0x226c); }
    static QChar notEquivalentTo()                          { return QChar(0x226d); }
    static QChar notLessThan()                              { return QChar(0x226e); }
    static QChar notGreaterThan()                           { return QChar(0x226f); }
    static QChar neitherLessThanNorEqualTo()                { return QChar(0x2270); }
    static QChar neitherGreaterThanNorEqualTo()             { return QChar(0x2271); }
    static QChar lessThanOrEquivalentTo()                   { return QChar(0x2272); }
    static QChar greaterThanOrEquivalentTo()                { return QChar(0x2273); }
    static QChar neitherLessThanNorEquivalentTo()           { return QChar(0x2274); }
    static QChar neitherGreaterThanNorEquivalentTo()        { return QChar(0x2275); }
    static QChar lessThanOrGreaterThan()                    { return QChar(0x2276); }
    static QChar greaterThanOrLessThan()                    { return QChar(0x2277); }
    static QChar neitherLessNorGreaterThan()                { return QChar(0x2278); }
    static QChar neitherGreaterNorLessThan()                { return QChar(0x2279); }
    static QChar precedes()                                 { return QChar(0x227a); }
    static QChar succeeds()                                 { return QChar(0x227b); }
    static QChar precedesOrEqualTo()                        { return QChar(0x227c); }
    static QChar succeedsOrEqualTo()                        { return QChar(0x227d); }
    static QChar precedesOrEquivalentTo()                   { return QChar(0x227e); }
    static QChar succeedsOrEquivalentTo()                   { return QChar(0x227f); }
    static QChar doesNotPrecede()                           { return QChar(0x2280); }
    static QChar doesNotSucceed()                           { return QChar(0x2281); }
    static QChar supersetOf()                               { return QChar(0x2283); }
    static QChar notSubsetOf()                              { return QChar(0x2284); }
    static QChar notSupersetOf()                            { return QChar(0x2285); }
    static QChar subsetOrEqualTo()                          { return QChar(0x2286); }
    static QChar supersetOrEqualTo()                        { return QChar(0x2287); }
    static QChar neitherSubsetNorEqualTo()                  { return QChar(0x2288); }
    static QChar neitherSupersetNorEqualTo()                { return QChar(0x2289); }
    static QChar subsetAndNotEqualTo()                      { return QChar(0x228a); }
    static QChar supersetAndNotEqualTo()                    { return QChar(0x228b); }
    static QChar multisetMultiplication()                   { return QChar(0x228d); }
    static QChar multiSetUnion()                            { return QChar(0x228e); }
    static QChar squareImageOf()                            { return QChar(0x228f); }
    static QChar squareOriginalOf()                         { return QChar(0x2290); }
    static QChar squareImageOfOrEqualTo()                   { return QChar(0x2291); }
    static QChar squareOriginalOfOrEqualTo()                { return QChar(0x2292); }
    static QChar squareCap()                                { return QChar(0x2293); }
    static QChar squareCup()                                { return QChar(0x2294); }
    static QChar circledPlus()                              { return QChar(0x2295); }
    static QChar circledMinus()                             { return QChar(0x2296); }
    static QChar circledTimes()                             { return QChar(0x2297); }
    static QChar circledDivisionSlash()                     { return QChar(0x2298); }
    static QChar circledDotOperator()                       { return QChar(0x2299); }
    static QChar circledRingOperator()                      { return QChar(0x229a); }
    static QChar circledAsteriskOperator()                  { return QChar(0x229b); }
    static QChar circledEquals()                            { return QChar(0x229c); }
    static QChar circledDash()                              { return QChar(0x229d); }
    static QChar squaredPlus()                              { return QChar(0x229e); }
    static QChar squaredMinus()                             { return QChar(0x229f); }
    static QChar squaredTimes()                             { return QChar(0x22a0); }
    static QChar squaredDotOperator()                       { return QChar(0x22a1); }
    static QChar precedesUnderRelation()                    { return QChar(0x22b0); }
    static QChar succeedsUnderRelation()                    { return QChar(0x22b1); }
    static QChar normalSubgroupOf()                         { return QChar(0x22b2); }
    static QChar containsAsNormalSubgroup()                 { return QChar(0x22b3); }
    static QChar normalSubgroupOrEqualTo()                  { return QChar(0x22b4); }
    static QChar containsAsNormalSubgroupOrEqualTo()        { return QChar(0x22b5); }
    static QChar nand()                                     { return QChar(0x22bc); }
    static QChar nor()                                      { return QChar(0x22bd); }
    static QChar diamondOperator()                          { return QChar(0x22c4); }
    static QChar starOperator()                             { return QChar(0x22c6); }
    static QChar divisionTimes()                            { return QChar(0x22c7); }
    static QChar bowTie()                                   { return QChar(0x22c8); }
    static QChar leftNormalFactorSemidirectProduct()        { return QChar(0x22c9); }
    static QChar rightNormalFactorSemidirectProduct()       { return QChar(0x22ca); }
    static QChar leftSemidirectProduct()                    { return QChar(0x22cb); }
    static QChar rightSemidirectProduct()                   { return QChar(0x22cc); }
    static QChar reversedTildeEquals()                      { return QChar(0x22cd); }
    static QChar curlyLogicalOr()                           { return QChar(0x22ce); }
    static QChar curlyLogicalAnd()                          { return QChar(0x22cf); }
    static QChar doubleSubset()                             { return QChar(0x22d0); }
    static QChar doubleSuperset()                           { return QChar(0x22d1); }
    static QChar doubleIntersection()                       { return QChar(0x22d2); }
    static QChar doubleUnion()                              { return QChar(0x22d3); }
    static QChar equalAndParallelTo()                       { return QChar(0x22d5); }
    static QChar lessThanWithDot()                          { return QChar(0x22d6); }
    static QChar greaterThanWithDot()                       { return QChar(0x22d7); }
    static QChar veryMuchLessThan()                         { return QChar(0x22d8); }
    static QChar veryMuchGreaterThan()                      { return QChar(0x22d9); }
    static QChar lessThanOrEqualToOrGreaterThan()           { return QChar(0x22da); }
    static QChar greaterThanOrEqualToOrLessThan()           { return QChar(0x22db); }
    static QChar equalToOrLessThan()                        { return QChar(0x22dc); }
    static QChar equalToOrGreaterThan()                     { return QChar(0x22dd); }
    static QChar equalToOrPrecedes()                        { return QChar(0x22de); }
    static QChar equalToOrSucceeds()                        { return QChar(0x22df); }
    static QChar doesNotPrecedeOrEqual()                    { return QChar(0x22e0); }
    static QChar doesNotSucceedOrEqual()                    { return QChar(0x22e1); }
    static QChar notSquareImageOfOrEqualTo()                { return QChar(0x22e2); }
    static QChar notSquareOriginalOrEqualTo()               { return QChar(0x22e3); }
    static QChar squareImageOfAndNotEqualTo()               { return QChar(0x22e4); }
    static QChar squareOriginalOfAndNotEqualTo()            { return QChar(0x22e5); }
    static QChar lessThanButNotEquivalentTo()               { return QChar(0x22e6); }
    static QChar greaterThanButNotEquivalentTo()            { return QChar(0x22e7); }
    static QChar precedesButNotEquivalentTo()               { return QChar(0x22e8); }
    static QChar succeedsButNotEquivalentTo()               { return QChar(0x22e9); }
    static QChar notNormalSubgroupOf()                      { return QChar(0x22ea); }
    static QChar doesNotContainAsNormalSubgroup()           { return QChar(0x22eb); }
    static QChar notNormalSubgroupOfOrEqualTo()             { return QChar(0x22ec); }
    static QChar doesNotContainAsNormalSubgroupOrEqualTo()  { return QChar(0x22ed); }
    static QChar elementOf()                                { return QChar(0x2208); }
    static QChar notAnElementOf()                           { return QChar(0x2209); }
    static QChar elementOfWithStroke()                      { return QChar(0x22f2); }
    static QChar elementOfWithStrokeToBar()                 { return QChar(0x22f3); }
    static QChar elementOfWithDot()                         { return QChar(0x22f5); }
    static QChar elementOfWithOverbar()                     { return QChar(0x22f6); }
    static QChar elementOfWithUnderbar()                    { return QChar(0x22f8); }
    static QChar elementOfWithDoubleStroke()                { return QChar(0x22f9); }
    static QChar containsAsMember()                         { return QChar(0x220b); }
    static QChar doesNotContainAsMember()                   { return QChar(0x220c); }
    static QChar containsWithStroke()                       { return QChar(0x22fa); }
    static QChar containsWithStrokeFromBar()                { return QChar(0x22fb); }
    static QChar containsWithOverbar()                      { return QChar(0x22fd); }
    static QChar plusSignWithDotAbove()                     { return QChar(0x2a22); }
    static QChar plusSignWithHatAbove()                     { return QChar(0x2a23); }
    static QChar plusSignWithTildeAbove()                   { return QChar(0x2a24); }
    static QChar plusSignWithDotBelow()                     { return QChar(0x2a25); }
    static QChar plusSignWithTildeBelow()                   { return QChar(0x2a26); }
    static QChar plusSignWithSubscriptTwo()                 { return QChar(0x2a27); }
    static QChar plusSignWithBlackTriangle()                { return QChar(0x2a28); }
    static QChar minusSignWithCommaAbove()                  { return QChar(0x2a29); }
    static QChar minusSignWithDotBelow()                    { return QChar(0x2a2a); }
    static QChar minusSignWithFallingDots()                 { return QChar(0x2a2b); }
    static QChar minusSignWithRisingDots()                  { return QChar(0x2a2c); }
    static QChar plusSignInLeftHalfCircle()                 { return QChar(0x2a2d); }
    static QChar plusSignInRightHalfCircle()                { return QChar(0x2a2e); }
    static QChar vectorOrCrossProduct()                     { return QChar(0x2a2f); }
    static QChar timesWithDotAbove()                        { return QChar(0x2a30); }
    static QChar timesWithUnderbar()                        { return QChar(0x2a31); }
    static QChar semidirectProductWithBottomClosed()        { return QChar(0x2a32); }
    static QChar smashProduct()                             { return QChar(0x2a33); }
    static QChar timesInLeftHalfCircle()                    { return QChar(0x2a34); }
    static QChar timesInRightHalfCircle()                   { return QChar(0x2a35); }
    static QChar circledTimesWithHatAbove()                 { return QChar(0x2a36); }
    static QChar timesInDoubleCircle()                      { return QChar(0x2a37); }
    static QChar circledDivisionSign()                      { return QChar(0x2a38); }
    static QChar plusSignInTriangle()                       { return QChar(0x2a39); }
    static QChar minusSignInTriangle()                      { return QChar(0x2a3a); }
    static QChar timesInTriangle()                          { return QChar(0x2a4b); }
    static QChar interiorProduct()                          { return QChar(0x2a3c); }
    static QChar righthandInteriorProduct()                 { return QChar(0x2a3d); }

    // special symbols
    static QChar emptySet()                         { return QChar(0x2205); }
    static QChar infinity()                         { return QChar(0x221e); }
    static QChar midlineHorizontalEllipsis()        { return QChar(0x22ef); }
    static QChar twoDotLeader()                     { return QChar(0x2025); }
    static QChar leftwardsArrow()                   { return QChar(0x2190); }
    static QChar rightwardsArrow()                  { return QChar(0x2192); }
    static QChar longLeftwardsArrow()               { return QChar(0x27f5); }
    static QChar longRightwardsArrow()              { return QChar(0x27f6); }
    static QChar rightwardsArrowFromBar()           { return QChar(0x21a6); }
    static QChar placeholder()                      { return QChar(0x25a3); }
    static QChar leftDoubleQuotationMark()          { return QChar(0x201c); }
    static QChar rightDoubleQuotationMark()         { return QChar(0x201d); }
    static QChar aleph()                            { return QChar(0x2135); }
    static QChar horizontalDots()                   { return QChar(0x2026); }
    static QChar diagonalDots()                     { return QChar(0x22f1); }
    static QChar rightDiagonalDots()                { return QChar(0x22f0); }
    static QChar verticalDots()                     { return QChar(0x22ee); }
    static QChar centeredDots()                     { return QChar(0x22ef); }
    static QChar iMath()                            { return QChar(0x0131); }
    static QChar jMath()                            { return QChar(0x0237); }
    static QChar degree()                           { return QChar(0x00b0); }
    static QChar degreeCelsius()                    { return QChar(0x2103); }
    static QChar degreeFahrenheit()                 { return QChar(0x2109); }

    // letters
    static QChar smallLetterHWithStroke()           { return QChar(0x0127); }
    static QChar smallLetterPi()                    { return QChar(0x03c0); }
    static QChar smallLetterEpsilon()               { return QChar(0x03b5); }
    static QChar subscriptSmallLetterA()            { return QChar(0x2090); }
    static QChar subscriptSmallLetterE()            { return QChar(0x2091); }
    static QChar subscriptSmallLetterO()            { return QChar(0x2092); }
    static QChar subscriptSmallLetterX()            { return QChar(0x2093); }
    static QChar subscriptSmallLetterH()            { return QChar(0x2095); }
    static QChar subscriptSmallLetterK()            { return QChar(0x2096); }
    static QChar subscriptSmallLetterL()            { return QChar(0x2097); }
    static QChar subscriptSmallLetterM()            { return QChar(0x2098); }
    static QChar subscriptSmallLetterN()            { return QChar(0x2099); }
    static QChar subscriptSmallLetterP()            { return QChar(0x209a); }
    static QChar subscriptSmallLetterS()            { return QChar(0x209b); }
    static QChar subscriptSmallLetterT()            { return QChar(0x209c); }

    // accents
    static QChar hatAccent()                        { return QChar(0x0302); }
    static QChar checkAccent()                      { return QChar(0x030c); }
    static QChar tildeAccent()                      { return QChar(0x0303); }
    static QChar acuteAccent()                      { return QChar(0x0301); }
    static QChar graveAccent()                      { return QChar(0x0300); }
    static QChar dotAccent()                        { return QChar(0x0307); }
    static QChar doubleDotAccent()                  { return QChar(0x0308); }
    static QChar tripleDotAccent()                  { return QChar(0x20db); }
    static QChar barAccent()                        { return QChar(0x0304); }
    static QChar vecAccent()                        { return QChar(0x20d7); }

    // composite symbols
    static QChar topHalfIntegral()                  { return QChar(0x2320); }
    static QChar bottomHalfIntegral()               { return QChar(0x2321); }
    static QChar summationTop()                     { return QChar(0x23b2); }
    static QChar summationBottom()                  { return QChar(0x23b3); }

    // brackets
    static QChar leftCeilingBracket()               { return QChar(0x2308); }
    static QChar rightCeilingBracket()              { return QChar(0x2309); }
    static QChar leftFloorBracket()                 { return QChar(0x230a); }
    static QChar rightFloorBracket()                { return QChar(0x230b); }
    static QChar leftParenthesisUpperHook()         { return QChar(0x239b); }
    static QChar leftParenthesisExtension()         { return QChar(0x239c); }
    static QChar leftParenthesisLowerHook()         { return QChar(0x239d); }
    static QChar rightParenthesisUpperHook()        { return QChar(0x239e); }
    static QChar rightParenthesisExtension()        { return QChar(0x239f); }
    static QChar rightParenthesisLowerHook()        { return QChar(0x23a0); }
    static QChar leftSquareBracketUpperCorner()     { return QChar(0x23a1); }
    static QChar leftSquareBracketExtension()       { return QChar(0x23a2); }
    static QChar leftSquareBracketLowerCorner()     { return QChar(0x23a3); }
    static QChar rightSquareBracketUpperCorner()    { return QChar(0x23a4); }
    static QChar rightSquareBracketExtension()      { return QChar(0x23a5); }
    static QChar rightSquareBracketLowerCorner()    { return QChar(0x23a6); }
    static QChar leftCurlyBracketUpperHook()        { return QChar(0x23a7); }
    static QChar leftCurlyBracketMiddlePiece()      { return QChar(0x23a8); }
    static QChar leftCurlyBracketLowerHook()        { return QChar(0x23a9); }
    static QChar leftCurlyBracketExtension()        { return QChar(0x23aa); }
    static QChar rightCurlyBracketUpperHook()       { return QChar(0x23ab); }
    static QChar rightCurlyBracketMiddlePiece()     { return QChar(0x23ac); }
    static QChar rightCurlyBracketLowerHook()       { return QChar(0x23ad); }
    static QChar rightCurlyBracketExtension()       { return QChar(0x23ae); }
    static QChar leftAngleBracket()                 { return QChar(0x27e8); }
    static QChar rightAngleBracket()                { return QChar(0x27e9); }
    static QChar horizontalLineExtension()          { return QChar(0x23af); }
    static QChar verticalLineExtension()            { return QChar(0x23d0); }
    static QChar upperLeftCurlyBracketSection()     { return QChar(0x23b0); }
    static QChar lowerLeftCurlyBracketSection()     { return QChar(0x23b1); }
    static QChar leftWhiteParenthesis()             { return QChar(0x2985); }
    static QChar rightWhiteParenthesis()            { return QChar(0x2986); }
    static QChar leftWhiteSquareBracket()           { return QChar(0x27e6); }
    static QChar rightWhiteSquareBracket()          { return QChar(0x27e7); }
    static QChar leftWhiteCurlyBracket()            { return QChar(0x2983); }
    static QChar rightWhiteCurlyBracket()           { return QChar(0x2984); }
    static QChar superscriptLeftParenthesis()       { return QChar(0x207d); }
    static QChar superscriptRightParenthesis()      { return QChar(0x207e); }
    static QChar subscriptLeftParenthesis()         { return QChar(0x208d); }
    static QChar subscriptRightParenthesis()        { return QChar(0x208e); }
    static QChar straightBracket()                  { return QChar(0x007c); }
    static QChar doubleStraightBracket()            { return QChar(0x2016); }
};

#endif // GLYPHS_H
