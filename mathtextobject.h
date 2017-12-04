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

#ifndef MATHTEXTOBJECT_H
#define MATHTEXTOBJECT_H

#include <QObject>
#include <QTextObjectInterface>

class MathTextObject : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    enum { Id = QTextFormat::UserObject + 1 };
    enum { Data = 1 };
    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format);
    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                    int posInDocument, const QTextFormat &format);
};

#endif // MATHTEXTOBJECT_H
