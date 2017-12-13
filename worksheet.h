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
    QString m_fileName;
    QString m_language;

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
        Label = 4,
        AssociatedFrame = 5,
        Flags = 6,
    };

    enum TableFlag {
        None = 0x0,
        Numeric = 0x1,
    };
    Q_DECLARE_FLAGS(TableFlags, TableFlag)

    enum FrameSubtype { CasInput, CasOutput, Heading };

    Worksheet(GIAC_CONTEXT, QObject *parent = 0);

    void insertHeadingFrame(QTextCursor &cursor, int level);
    void insertCasInputFrame(QTextCursor &cursor);
    void insertTable(QTextCursor &cursor, int rows, int columns, int headerRowCount, int flags);
    void insertImage(QTextCursor &cursor, QString name);
    void removeFrame(QTextFrame *frame);
    bool isHeadingFrame(QTextFrame *frame, int &level);
    bool isCasInputFrame(QTextFrame *frame);
    bool isCasOutputFrame(QTextFrame *frame);
    bool isTable(QTextFrame *frame, int &flags);

    inline bool isUnnamed() { return m_fileName.length() == 0; }
    inline const QString fileName() { return m_fileName; }
    inline void setFileName(QString fname) { m_fileName = fname; }

signals:
    void stylingEnableChanged(bool enabled);
    void alignEnableChanged(bool enabled);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(Worksheet::TableFlags)

#endif // DOCUMENT_H
