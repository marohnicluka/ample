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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QTextDocument>
#include <QTextCursor>
#include <QFont>
#include <QFontMetrics>
#include <QString>
#include <QStringList>
#include <QTextFrame>
#include <QTextTable>
#include <QList>
#include <qmath.h>
#include <giac/giac.h>
#include "giachighlighter.h"

using namespace giac;

class GiacHighlighter;
class DocumentCounter;

class Worksheet : public QTextDocument
{
    Q_OBJECT

private:
    const context *gcontext;
    GiacHighlighter *ghighlighter;

    QString frameText(QTextFrame *frame);
    QTextFrame *insertCasOutputFrame(QTextFrame *inputFrame);

private slots:
    void casInputDestroyed(QObject *casInput);
    void casOutputDestroyed(QObject *casOutput);
    void on_modificationChanged(bool changed);
    void updateEnumeration(QObject *deletedObject = nullptr);

public:
    enum PropertyId {
        Subtype = 1,
        Level = 2,
        Editable = 3,
        Number = 4,
        CasOutputFrame = 5,
        CasInputFrame = 6,
        Flags = 7,
    };

    enum TableProperty {
        HasHeaderRow = 0x1,
        HasHeaderColumn = 0x2,
        Numeric = 0x4,
    };
    Q_DECLARE_FLAGS(TableProperties, TableProperty)

    enum FrameSubtype { CasInput, CasOutput, Heading };

    Worksheet(GIAC_CONTEXT, QObject *parent = 0);

    QString fileName;
    QString language;
    bool worksheetMode;

    void insertHeadingFrame(QTextCursor &cursor, int level);
    void insertCasInputFrame(QTextCursor &cursor);
    void insertTable(QTextCursor &cursor, int rows, int columns, bool isNumeric,
                     bool hasHeaderRow, bool hasHeaderColumn, QBrush &headerBgColor);
    void insertImage(QTextCursor &cursor, QString name);
    void removeFrame(QTextFrame *frame);
    bool isHeadingFrame(QTextFrame *frame, int &level);
    bool isCasInputFrame(QTextFrame *frame);
    bool isCasOutputFrame(QTextFrame *frame);
    bool isTable(QTextFrame *frame, int &flags);

signals:
    void stylingEnableChanged(bool enabled);
    void alignEnableChanged(bool enabled);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(Document::TableProperties)

#endif // DOCUMENT_H
