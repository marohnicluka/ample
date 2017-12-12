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

#include <QFont>
#include <QFontMetrics>

class Glyphs
{
private:
    QFont m_font;

public:
    Glyphs(const QFont &font);

    static inline QChar emQuad() { return QChar(0x2001); }
};

#endif // GLYPHS_H
