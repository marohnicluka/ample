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
    static QString defaultSerifFontFamily;
    static QString defaultSansFontFamily;
    static QString defaultMonoFontFamily;

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

public:
    enum PropertyId { ParagraphStyleId = 1, TextStyleId = 2, CounterTypeId = 3, CounterTagId = 4 };
    enum ParagraphType { TextBody = 1, Title = 2, Section = 3, Subsection = 4, List = 5, NumberedList = 6 };
    enum TextType { NormalTextStyle, MathTextStyle };
    enum CounterType {
        None, SectionCounterType, SubsectionCounterType, Equation, Figure, Table, Axiom, Definition,
        Proposition, Lemma, Theorem, Corollary, Algorithm, Remark, Note, Example, Exercise, Problem, User
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
    static qreal paragraphMargin;

    inline qreal fontSize(int level) { return baseFontSize * qPow(GROUND_RATIO, level); }
    void newCounter(int type, int baseType, const QString &name,
                    const QString &fontFamily, qreal fontSize, QFont::Weight weight, bool italic,
                    const QString &prefix, const QString &suffix);
    void setCounterFont(int type, const QString &fontFamily, qreal fontSize, QFont::Weight weight, bool italic);
    void setCounterBase(int type, int baseType);
    void setCounterPrefixAndSuffix(int type, const QString &newPrefix, const QString &newSuffix);
    const QFont counterFont(int type);
    void updateEnumeration();
    void blockToParagraph(QTextCursor &cursor);
    static void applyFormatToEndOfBlock(const QTextCursor &cursor);
    static bool isHeading(const QTextBlock &block);

signals:
    void fileNameChanged(const QString newName);

};

#endif // DOCUMENT_H
