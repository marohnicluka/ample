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

#include <QTextOption>
#include "document.h"
#include "glyphs.h"

using namespace giac;

Document::Document(GIAC_CONTEXT, QObject *parent) : QTextDocument(parent)
{
    gcontext = contextptr;
    ghighlighter = new GiacHighlighter(this);
    worksheetMode = false;
    baseFontSize = 12.0;
    paragraphMargin = baseFontSize / GROUND_RATIO;
    titleFont = QFont("FreeSans", fontSize(3), QFont::Normal, false);
    sectionFont = QFont("LiberationSans", fontSize(2), QFont::Bold, false);
    subsectionFont = QFont("LiberationSans", fontSize(1), QFont::Bold, false);
    textFont = QFont("FreeSerif", fontSize(0), QFont::Normal, false);
    codeFont = QFont("FreeMono", fontSize(0), QFont::Normal, false);
    QTextFrameFormat rootFrameFormat = rootFrame()->frameFormat();
    rootFrameFormat.setLeftMargin(paragraphMargin);
    rootFrameFormat.setRightMargin(paragraphMargin);
    rootFrame()->setFrameFormat(rootFrameFormat);
    createDefaultCounters();
}

bool Document::isHeading(const QTextBlock &block)
{
    int type = block.blockFormat().intProperty(ParagraphStyleId);
    return type == Document::Title || type == Document::Section || type == Document::Subsection;
}

void Document::newCounter(int type, int baseType, const QString &name, const QFont &font,
                          const QString &prefix, const QString &suffix)
{
    if (baseType != None && !registeredCounterTypes.contains(baseType))
    {
        qWarning("Error: base counter type not registered");
        return;
    }
    Counter counter;
    counter.name = name;
    counter.font = font;
    counter.prefix = prefix;
    counter.suffix = suffix;
    counter.baseType = baseType;
    counters[type] = counter;
    registeredCounterTypes.append(type);
}

void Document::createDefaultCounters()
{
    QFont textFontBold(textFont);
    textFontBold.setWeight(QFont::Bold);
    QFont textFontItalic(textFont);
    textFontItalic.setItalic(true);
    newCounter(SectionCounterType, None, tr("Section"), sectionFont, "", "");
    newCounter(SubsectionCounterType, SectionCounterType, tr("Subsection"), subsectionFont, "", "");
    newCounter(Equation, SectionCounterType, tr("Equation"), textFont, "(", ")");
    newCounter(Figure, SectionCounterType, tr("Figure"), textFont, "", "");
    newCounter(Table, SectionCounterType, tr("Table"), textFont, "", "");
    newCounter(Axiom, SectionCounterType, tr("Axiom"), textFontBold, tr("Axiom"), ".");
    newCounter(Definition, SectionCounterType, tr("Definition"), textFontBold, tr("Definition"), ".");
    newCounter(Proposition, SectionCounterType, tr("Proposition"), textFontBold, tr("Proposition"), ".");
    newCounter(Lemma, SectionCounterType, tr("Lemma"), textFontBold, tr("Lemma"), ".");
    newCounter(Theorem, SectionCounterType, tr("Theorem"), textFontBold, tr("Theorem"), ".");
    newCounter(Corollary, SectionCounterType, tr("Corollary"), textFontBold, tr("Corollary"), ".");
    newCounter(Algorithm, SectionCounterType, tr("Algorithm"), textFontBold, tr("Algorithm"), ".");
    newCounter(Problem, SectionCounterType, tr("Problem"), textFontBold, tr("Problem"), ".");
    newCounter(Remark, SectionCounterType, tr("Remark"), textFontItalic, tr("Remark"), ".");
    newCounter(Note, SectionCounterType, tr("Note"), textFontItalic, tr("Note"), ".");
    newCounter(Example, SectionCounterType, tr("Example"), textFontItalic, tr("Example"), ".");
    newCounter(Exercise, SectionCounterType, tr("Exercise"), textFontItalic, tr("Exercise"), ".");
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

const QFont Document::counterFont(int type)
{
    return counters[type].font;
}

void Document::updateEnumeration()
{
    QTextCursor cursor(this);
    cursor.movePosition(QTextCursor::Start);
    for (QMap<int, Counter>::iterator it = counters.begin(); it != counters.end(); ++it)
        it->count = 0;
    do
    {
        int type;
        QTextBlockFormat blockFormat = cursor.blockFormat();
        if (blockFormat.hasProperty(CounterTypeId) &&
                (type = blockFormat.intProperty(CounterTypeId)) != None)
        {
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

void Document::blockToParagraph(QTextCursor &cursor)
{
    QTextCharFormat format;
    QTextBlockFormat blockFormat = cursor.blockFormat();
    format.setFont(textFont);
    applyFormatToBlock(cursor, format, true);
    cursor.setBlockCharFormat(format);
    blockFormat.setProperty(Document::ParagraphStyleId, Document::TextBody);
    blockFormat.setTextIndent(0.0);
    if (blockFormat.hasProperty(Document::CounterTypeId))
        blockFormat.setProperty(Document::CounterTypeId, Document::None);
    cursor.mergeBlockFormat(blockFormat);
}

void Document::applyFormatToBlock(const QTextCursor &cursor, const QTextCharFormat &format, bool fromStart)
{
    QTextCursor pCursor(cursor);
    if (fromStart)
        pCursor.movePosition(QTextCursor::StartOfBlock);
    pCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    if (pCursor.hasSelection())
        pCursor.mergeCharFormat(format);
}
