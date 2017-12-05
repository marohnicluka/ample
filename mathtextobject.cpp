/*
 * This file is part of Giac Qt.
 *
 * Giac Qt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Giac Qt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Giac Qt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mathtextobject.h"

QSizeF MathTextObject::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    return QSizeF(1,1);
}

void MathTextObject::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                                int posInDocument, const QTextFormat &format)
{

}
