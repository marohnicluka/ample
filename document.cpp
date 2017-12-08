/*
 * This file is part of Ample Editor.
 *
 * Ample Editor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ample Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ample Editor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "document.h"
#include "glyphs.h"

using namespace giac;

QString Document::defaultSerifFontFamily = QString("FreeSerif");
QString Document::defaultSansFontFamily = QString("FreeSans");
QString Document::defaultMonoFontFamily = QString("FreeMono");

Document::Document(GIAC_CONTEXT, QObject *parent) : QTextDocument(parent)
{
    gcontext = contextptr;
    ghighlighter = new GiacHighlighter(this);
    worksheetMode = false;
    baseFontSize = 12.0;
    titleFont = QFont(defaultSerifFontFamily, fontSize(3), QFont::Normal, false);
    sectionFont = QFont(defaultSansFontFamily, fontSize(2), QFont::Bold, false);
    subsectionFont = QFont(defaultSansFontFamily, fontSize(1), QFont::Bold, false);
    textFont = QFont(defaultSerifFontFamily, fontSize(0), QFont::Normal, false);
    codeFont = QFont(defaultMonoFontFamily, fontSize(0), QFont::Normal, false);
}

bool Document::isHeading(const QTextBlock &block)
{
    int type = block.blockFormat().intProperty(ParagraphStyleId);
    return type == Document::Title || type == Document::Section || type == Document::Subsection;
}

void Document::newCounter(int type, int baseType, const QString &name,
                          const QString &fontFamily, qreal fontSize, QFont::Weight weight, bool italic,
                          const QString &prefix, const QString &suffix)
{
    if (!registeredCounterTypes.contains(baseType))
    {
        qWarning("Error: base counter type not registered");
        return;
    }
    Counter counter;
    counter.name = name;
    counter.font = QFont(fontFamily, fontSize, weight, italic);
    counter.prefix = prefix;
    counter.suffix = suffix;
    counter.baseType = baseType;
    counters[type] = counter;
    registeredCounterTypes.append(type);
}

void Document::createDefaultCounters()
{
    newCounter(SectionCounterType, None, tr("Section"),
               defaultSansFontFamily, fontSize(2), QFont::Bold, false, "", "");
    newCounter(SubsectionCounterType, SectionCounterType, tr("Subsection"),
               defaultSansFontFamily, fontSize(1), QFont::Bold, false, "", "");
    newCounter(Equation, SectionCounterType, tr("Equation"),
               defaultSerifFontFamily, fontSize(0), QFont::Normal, false, "(", ")");
    newCounter(Figure, SectionCounterType, tr("Figure"),
               defaultSerifFontFamily, fontSize(-1), QFont::Normal, false, "", "");
    newCounter(Table, SectionCounterType, tr("Table"),
               defaultSerifFontFamily, fontSize(-1), QFont::Normal, false, "", "");
    newCounter(Axiom, SectionCounterType, tr("Axiom"),
               defaultSerifFontFamily, fontSize(0), QFont::Bold, false, tr("Axiom"), ".");
    newCounter(Definition, SectionCounterType, tr("Definition"),
               defaultSerifFontFamily, fontSize(0), QFont::Bold, false, tr("Definition"), ".");
    newCounter(Proposition, SectionCounterType, tr("Proposition"),
               defaultSerifFontFamily, fontSize(0), QFont::Bold, false, tr("Proposition"), ".");
    newCounter(Lemma, SectionCounterType, tr("Lemma"),
               defaultSerifFontFamily, fontSize(0), QFont::Bold, false, tr("Lemma"), ".");
    newCounter(Theorem, SectionCounterType, tr("Theorem"),
               defaultSerifFontFamily, fontSize(0), QFont::Bold, false, tr("Theorem"), ".");
    newCounter(Corollary, SectionCounterType, tr("Corollary"),
               defaultSerifFontFamily, fontSize(0), QFont::Bold, false, tr("Corollary"), ".");
    newCounter(Algorithm, SectionCounterType, tr("Algorithm"),
               defaultSerifFontFamily, fontSize(0), QFont::Bold, false, tr("Algorithm"), ".");
    newCounter(Problem, SectionCounterType, tr("Problem"),
               defaultSerifFontFamily, fontSize(0), QFont::Bold, false, tr("Problem"), ".");
    newCounter(Remark, SectionCounterType, tr("Remark"),
               defaultSerifFontFamily, fontSize(0), QFont::Normal, true, tr("Remark"), ".");
    newCounter(Note, SectionCounterType, tr("Note"),
               defaultSerifFontFamily, fontSize(0), QFont::Normal, true, tr("Note"), ".");
    newCounter(Example, SectionCounterType, tr("Example"),
               defaultSerifFontFamily, fontSize(0), QFont::Normal, true, tr("Example"), ".");
    newCounter(Exercise, SectionCounterType, tr("Exercise"),
               defaultSerifFontFamily, fontSize(0), QFont::Normal, true, tr("Exercise"), ".");
}

void Document::setCounterFont(int type, const QString &fontFamily, qreal fontSize, QFont::Weight weight, bool italic)
{
    Counter &counter = counters[type];
    counter.font = QFont(fontFamily, fontSize, weight, italic);
}

void Document::setCounterBase(int type, int baseType)
{
    counters[type].baseType = baseType;
}

void Document::setCounterPrefixAndSuffix(int type, const QString &newPrefix, const QString &newSuffix)
{
    counters[type].prefix = QString(newPrefix);
    counters[type].suffix = QString(newSuffix);
}

QString Document::counterCurrentNumber(int type)
{
    QStringList list;
    Counter &counter = counters[type];
    list.append(QString::number(counter.count));
    switch (counter.baseType)
    {
    case SubsectionCounterType:
        list.prepend(QString::number(counters[SubsectionCounterType].count));
    case SectionCounterType:
        list.prepend(QString::number(counters[SectionCounterType].count));
        break;
    default:
        return list.front();
    }
    return list.join(".");
}

void Document::updateEnumeration()
{
    QTextCursor cursor(this);
    cursor.movePosition(QTextCursor::Start);
    for (QMap<int, Counter>::iterator it = counters.begin(); it != counters.end(); ++it)
        it->count = 0;
    do
    {
        QTextBlockFormat blockFormat = cursor.blockFormat();
        if (blockFormat.hasProperty(CounterTypeId))
        {
            int type = blockFormat.intProperty(CounterTypeId);
            Counter &counter = counters[type];
            ++counter.count;
            QString counterTag = counterCurrentNumber(type);
            counterTag.prepend(counter.prefix + " ");
            counterTag.append(counter.suffix);
            blockFormat.setProperty(CounterTagId, counterTag);
            QFontMetrics fontMetrics(counter.font);
            int indent = fontMetrics.width(counterTag + Glyphs::emQuad());
            blockFormat.setTextIndent(indent);
            cursor.mergeBlockFormat(blockFormat);
        }
    }
    while (cursor.movePosition(QTextCursor::NextBlock));
}
