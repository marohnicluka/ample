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
#include <QFont>
#include <QFontMetrics>
#include <QMap>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QTextFrame>
#include <QTextTable>
#include <QList>
#include <qmath.h>
#include <giac/giac.h>
#include "giachighlighter.h"

#define GROUND_RATIO 1.32471795724

using namespace giac;

class GiacHighlighter;
class DocumentCounter;

class Document : public QTextDocument
{
    Q_OBJECT

private:
    const context *gcontext;
    GiacHighlighter *ghighlighter;
    bool unnamed;
    QString lastTitle;
    QTimer *timer;
    QTextFrame *titleFrame;
    QTextTable *authorsTable;

    struct Counter
    {
        QString name;
        QFont font;
        QString prefix;
        QString suffix;
        int baseType;
        int count;
    };

    QMap<int, Counter> counters;
    QList<int> registeredCounterTypes;
    void createDefaultCounters();
    QString counterCurrentNumber(int type);
    QString getCurrentTitle();
    QList<QString> getFrameLines(QTextFrame *frame);

private slots:
    void updateProperties();
    void on_modificationChanged(bool changed);

public:
    struct Author
    {
        QString name;
        QStringList affiliation;
        QStringList emails;
    };

    enum PropertyId { ParagraphTypeId = 1, TextStyleId = 2, CounterTypeId = 3, CounterTagId = 4, FrameTypeId = 5 };
    enum ParagraphType { TextBody = 1, Title = 2, Section = 3, Subsection = 4, List = 5, NumberedList = 6 };
    enum FrameType { AffiliationFrame = 1, EmailFrame = 2 };
    enum TextStyle { NormalTextStyle, MathTextStyle };
    enum CounterType {
        None, SectionCounter, SubsectionCounter, Equation, Figure, Table, Axiom, Definition, Proposition,
        Lemma, Theorem, Corollary, Algorithm, Remark, Note, Example, Exercise, Problem, UserCounter
    };

    Document(GIAC_CONTEXT, QObject *parent = 0);

    QString fileName;
    QString language;
    bool worksheetMode;
    QFont titleFont;
    QFont sectionFont;
    QFont subsectionFont;
    QFont textFont;
    QFont codeFont;
    qreal baseFontSize;
    qreal paragraphMargin;

    qreal fontSize(int level, qreal scale = 1.0);
    void newCounter(int type, int baseType, const QString &name, const QFont &font,
                    const QString &prefix, const QString &suffix);
    void setCounterFont(int type, const QString &fontFamily, qreal fontSize, QFont::Weight weight, bool italic);
    void setCounterBase(int type, int baseType);
    void setCounterPrefixAndSuffix(int type, const QString &newPrefix, const QString &newSuffix);
    const QFont counterFont(int type);
    void updateEnumeration();
    void blockToParagraph(QTextCursor &cursor);
    void insertTitleFrame();
    void addAuthor();
    void addAffiliation();
    void addEmail();
    int numberOfAuthors();
    Author nthAuthor(int n);
    inline bool isEditingTitle(const QTextCursor &cursor) { return cursor.currentFrame() == titleFrame; }
    inline bool isEditingAuthorName(const QTextCursor &cursor) { return cursor.currentFrame() == authorsTable; }
    inline void setUnnamed(bool yes) { unnamed = yes; }
    inline bool isUnnamed() { return unnamed; }
    inline qreal paragraphSkip() { return QFontMetrics(textFont).lineSpacing(); }
    inline bool hasTitleFrame() { return titleFrame != nullptr; }
    const QTextCursor firstTitleCursorPosition();
    static void applyFormatToBlock(const QTextCursor &originalCursor, const QTextCharFormat &format, bool fromStart);
    static bool isHeading(const QTextBlock &block);
    static qreal groundRatioPower(int exponent);

signals:
    void fileNameChanged(const QString &newFileName);
    void titleChanged(const QString &newTitle);

};

#endif // DOCUMENT_H
