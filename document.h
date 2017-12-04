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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QTextDocument>
#include <giac/giac.h>
#include "giachighlighter.h"

using namespace giac;

class GiacHighlighter;

class Document : public QTextDocument
{
    Q_OBJECT

private:
    const context *gcontext;
    GiacHighlighter *ghighlighter;
public:
    Document(GIAC_CONTEXT, QObject *parent = 0);
    QString fileName;
    QString language;
    bool worksheetMode;
    struct Style {
        QString textBodyFontFamily;
        QString headingsFontFamily;
        QString casInputFontFamily;
        qreal fontPointSize;
    };
    Style style;

signals:
    void fileNameChanged(const QString newName);
};

#endif // DOCUMENT_H
