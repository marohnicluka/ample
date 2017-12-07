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
#include <QTextCursor>
#include <qmath.h>
#include <giac/giac.h>
#include "giachighlighter.h"
#include "documentcounter.h"

using namespace giac;

class GiacHighlighter;
class DocumentCounter;

class Document : public QTextDocument
{
    Q_OBJECT

private:
    const context *gcontext;
    GiacHighlighter *ghighlighter;
    DocumentCounter *sectionCounter;
    DocumentCounter *subsectionCounter;
    DocumentCounter *equationCounter;
    DocumentCounter *figureCounter;
    DocumentCounter *tableCounter;

public:
    enum PropertyId { ParagraphStyleId = 1, TextStyleId = 2 };
    enum ParagraphType { TextBody = 1, Title = 2, Section = 3, Subsection = 4, List = 5, NumberedList = 6 };
    enum TextType { NormalTextStyle = 0, MathTextStyle = 1 };

    struct Style
    {
        QString textBodyFontFamily;
        QString headingsFontFamily;
        QString casInputFontFamily;
        qreal fontPointSize;
        qreal groundRatio;
    };
    Style style;

    Document(GIAC_CONTEXT, QObject *parent = 0);
    QString fileName;
    QString language;
    bool worksheetMode;

    inline qreal headingSize(int level) { return style.fontPointSize * qPow(style.groundRatio, level); }
    static bool isHeading(const QTextBlock &block);

signals:
    void fileNameChanged(const QString newName);

};

#endif // DOCUMENT_H
