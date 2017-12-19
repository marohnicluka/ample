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

MathGlyphs::MathGlyphs(const QFont &font)
    : QFontMetricsF(font)
{
    smallGreekLetterNames << "alpha" << "beta" << "gamma" << "delta" << "epsilon" << "zeta" << "eta" << "theta"
                          << "iota" << "kappa" << "lambda" << "mu" << "nu" << "xi" << "omicron" << "pi" << "rho"
                          << "sigma" << "tau" << "upsilon" << "phi" << "chi" << "psi" << "omega";
    capitalGreekLetterNames << "Alpha" << "Beta" << "Gamma" << "Delta" << "Epsilon" << "Zeta" << "Eta" << "Theta"
                            << "Iota" << "Kappa" << "Lambda" << "Mu" << "Nu" << "Xi" << "Omicron" << "Pi" << "Rho"
                            << "Sigma" << "Tau" << "Upsilon" << "Phi" << "Chi" << "Psi" << "Omega";
}

bool MathGlyphs::isGreekLetter(const QString &name, QChar &chr)
{
    for (int i = 0; i < 24; ++i)
    {
        if (name == smallGreekLetterNames.at(i))
        {
            if (i > 16)
                ++i;
            chr = QChar(0x03b1 + i);
            return true;
        }
        if (name == capitalGreekLetterNames.at(i))
        {
            if (i > 16)
                ++i;
            chr = QChar(0x0391 + i);
            return true;
        }
    }
    return false;
}

QChar MathGlyphs::letterToMath(QChar letter, LetterType letterType, bool bold, bool italic)
{
    ushort code, letterCode = letter.unicode();
    bool isCapital = letter.isUpper();
    if (letterType == Greek)
        letterCode -= isCapital ? 0x0391 : 0x03b1;
    else
        letterCode -= isCapital ? 0x0041 : 0x0061;
    code = letterCode;
    switch (letterType)
    {
    case Serif:
        if (bold && italic)
            code += isCapital ? 0x1d468 : 0x1d482;
        else if (bold)
            code += isCapital ? 0x1d400 : 0x1d41a;
        else if (italic)
        {
            if (letter == 'h')
                code = 0x210e;
            else
                code += isCapital ? 0x1d434 : 0x1d44e;
        }
        else
            return letter;
        break;
    case Sans:
        if (bold && italic)
            code += isCapital ? 0x1d63c : 0x1d656;
        if (bold)
            code += isCapital ? 0x1d5d4 : 0x1d5ee;
        if (italic)
            code += isCapital ? 0x1d608 : 0x1d622;
        else
            code += isCapital ? 0x1d5a0 : 0x1d5ba;
        break;
    case Mono:
        code += isCapital ? 0x1d670 : 0x1d68a;
        break;
    case Script:
        if (bold)
            code += isCapital ? 0x1d4d0 : 0x1d4ea;
        else switch (letter.unicode())
        {
        case 'B':
            code = 0x212c;
            break;
        case 'E':
            code = 0x2130;
            break;
        case 'F':
            code = 0x2131;
            break;
        case 'H':
            code = 0x210b;
            break;
        case 'I':
            code = 0x2110;
            break;
        case 'L':
            code = 0x2112;
            break;
        case 'M':
            code = 0x2133;
            break;
        case 'R':
            code = 0x211b;
            break;
        case 'e':
            code = 0x212f;
            break;
        case 'g':
            code = 0x210a;
            break;
        case 'o':
            code = 0x2134;
            break;
        default:
            code += isCapital ? 0x1d49c : 0x1d4b6;
        }
        break;
    case Fraktur:
        if (bold)
            code += isCapital ? 0x1d56c : 0x1d586;
        else switch (letter.unicode())
        {
        case 'C':
            code = 0x212d;
            break;
        case 'H':
            code = 0x210c;
            break;
        case 'I':
            code = 0x2111;
            break;
        case 'R':
            code = 0x211c;
            break;
        case 'Z':
            code = 0x2128;
            break;
        default:
            code += isCapital ? 0x1d504 : 0x1d51e;
        }
        break;
    case DoubleStruck:
        switch (letter.unicode())
        {
        case 'C':
            code = 0x2102;
            break;
        case 'H':
            code = 0x210d;
            break;
        case 'N':
            code = 0x2115;
            break;
        case 'P':
            code = 0x2119;
            break;
        case 'Q':
            code = 0x211a;
            break;
        case 'R':
            code = 0x211d;
            break;
        case 'Z':
            code = 0x2124;
            break;
        default:
            code += isCapital ? 0x1d538 : 0x1d552;
        }
        break;
    case Greek:
        if (bold && italic)
            code += isCapital ? 0x1d71c : 0x1d736;
        if (bold)
            code += isCapital ? 0x1d6a8 : 0x1d6c2;
        if (italic)
            code += isCapital ? 0x1d6e2 : 0x1d6fc;
        else
            return letter;
        break;
    }
    return QChar(code);
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
