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

#include "mathglyphs.h"

QStringList MathGlyphs::smallGreekLetterNames = QStringList()
        << "alpha" << "beta" << "gamma" << "delta" << "epsilon" << "zeta" << "eta" << "theta"
        << "iota" << "kappa" << "lambda" << "mu" << "nu" << "xi" << "omicron" << "pi" << "rho"
        << "sigma" << "tau" << "upsilon" << "phi" << "chi" << "psi" << "omega";

QStringList MathGlyphs::capitalGreekLetterNames = QStringList()
        << "Alpha" << "Beta" << "Gamma" << "Delta" << "Epsilon" << "Zeta" << "Eta" << "Theta"
        << "Iota" << "Kappa" << "Lambda" << "Mu" << "Nu" << "Xi" << "Omicron" << "Pi" << "Rho"
        << "Sigma" << "Tau" << "Upsilon" << "Phi" << "Chi" << "Psi" << "Omega";

QString MathGlyphs::encodeUcs4(uint ucs4)
{
    if (QChar::requiresSurrogates(ucs4))
    {
        QChar chars[2];
        chars[0] = QChar::highSurrogate(ucs4);
        chars[1] = QChar::lowSurrogate(ucs4);
        return QString(chars, 2);
    }
    else return QChar(ucs4);
}

bool MathGlyphs::getGreekLetter(const QString &name, QChar &letter)
{
    for (ushort i = 0; i < 24; ++i)
    {
        if (name == smallGreekLetterNames.at(i))
        {
            if (i > 16)
                ++i;
            letter = QChar(i + 945);
            return true;
        }
        if (name == capitalGreekLetterNames.at(i))
        {
            if (i > 16)
                ++i;
            letter = QChar(i + 913);
            return true;
        }
    }
    return false;
}

QString MathGlyphs::letterToMath(QChar letter, LetterType letterType, bool bold, bool italic)
{
    uint code, letterCode = letter.unicode();
    bool isCapital = letter.isUpper();
    if (letterType == Greek)
        letterCode -= isCapital ? 913 : 945;
    else
        letterCode -= isCapital ? 65 : 97;
    code = letterCode;
    switch (letterType)
    {
    case Serif:
        if (bold && italic)
            code += isCapital ? 119912 : 119938;
        else if (bold)
            code += isCapital ? 119808 : 119834;
        else if (italic)
        {
            if (letter == 'h')
                code = 8462;
            else
                code += isCapital ? 119860 : 119886;
        }
        else
            return letter;
        break;
    case Sans:
        if (bold && italic)
            code += isCapital ? 120380 : 120406;
        if (bold)
            code += isCapital ? 120276 : 120302;
        if (italic)
            code += isCapital ? 120328 : 120354;
        else
            code += isCapital ? 120224 : 120250;
        break;
    case Mono:
        code += isCapital ? 120432 : 120458;
        break;
    case Script:
        if (bold)
            code += isCapital ? 120016 : 120042;
        else switch (letter.unicode())
        {
        case 'B':
            code = 8492;
            break;
        case 'E':
            code = 8496;
            break;
        case 'F':
            code = 8497;
            break;
        case 'H':
            code = 8459;
            break;
        case 'I':
            code = 8464;
            break;
        case 'L':
            code = 8466;
            break;
        case 'M':
            code = 8499;
            break;
        case 'R':
            code = 8475;
            break;
        case 'e':
            code = 8495;
            break;
        case 'g':
            code = 8458;
            break;
        case 'o':
            code = 8500;
            break;
        default:
            code += isCapital ? 119964 : 119990;
        }
        break;
    case Fraktur:
        if (bold)
            code += isCapital ? 120172 : 120198;
        else switch (letter.unicode())
        {
        case 'C':
            code = 8493;
            break;
        case 'H':
            code = 8460;
            break;
        case 'I':
            code = 8465;
            break;
        case 'R':
            code = 8476;
            break;
        case 'Z':
            code = 8488;
            break;
        default:
            code += isCapital ? 120068 : 120094;
        }
        break;
    case DoubleStruck:
        switch (letter.unicode())
        {
        case 'C':
            code = 8450;
            break;
        case 'H':
            code = 8461;
            break;
        case 'N':
            code = 8469;
            break;
        case 'P':
            code = 8473;
            break;
        case 'Q':
            code = 8474;
            break;
        case 'R':
            code = 8477;
            break;
        case 'Z':
            code = 8484;
            break;
        default:
            code += isCapital ? 120120 : 120146;
        }
        break;
    case Greek:
        if (bold && italic)
            code += isCapital ? 120604 : 120630;
        if (bold)
            code += isCapital ? 120488 : 120514;
        if (italic)
            code += isCapital ? 120546 : 120572;
        else
            return letter;
        break;
    }
    return encodeUcs4(code);
}

QString MathGlyphs::digitsToSuperscript(const QString &digits)
{
    QString text("");
    QString::const_iterator it;
    for (it = digits.begin(); it != digits.end(); ++it)
    {
        Q_ASSERT(it->isDigit());
        int value = it->digitValue();
        switch (value)
        {
        case 1:
            text.append(QChar(0x00b9));
            break;
        case 2:
            text.append(QChar(0x00b2));
            break;
        case 3:
            text.append(QChar(0x00b3));
            break;
        default:
            text.append(QChar(0x2070 + value));
        }
    }
    return text;
}

QString MathGlyphs::digitsToSubscript(const QString &digits)
{
    QString text("");
    QString::const_iterator it;
    for (it = digits.begin(); it != digits.end(); ++it)
    {
        Q_ASSERT(it->isDigit());
        text.append(QChar(0x2070 + it->digitValue()));
    }
    return text;
}
