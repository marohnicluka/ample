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

#include <QTextTableCell>
#include <QTextTableFormat>
#include <QTextLength>
#include <QVector>
#include "document.h"
#include "glyphs.h"

#define UPDATE_ON_MODIFICATION_TIMEOUT 300

using namespace giac;

Document::Document(GIAC_CONTEXT, QObject *parent) : QTextDocument(parent)
{
    gcontext = contextptr;
    ghighlighter = new GiacHighlighter(this);
    titleFrame = nullptr;
    timer = new QTimer(this);
    timer->setSingleShot(true);
    unnamed = true;
    worksheetMode = false;
    baseFontSize = 12.0;
    paragraphMargin = baseFontSize / GROUND_RATIO;
    titleFont = QFont("FreeSans", fontSize(3), QFont::Normal, false);
    sectionFont = QFont("LiberationSans", fontSize(2), QFont::Bold, false);
    subsectionFont = QFont("LiberationSans", fontSize(1), QFont::Bold, false);
    textFont = QFont("FreeSerif", fontSize(0), QFont::Normal, false);
    codeFont = QFont("FreeMono", fontSize(0), QFont::Normal, false);
    setDocumentMargin(baseFontSize);
    createDefaultCounters();
    connect(this, SIGNAL(modificationChanged(bool)), this, SLOT(on_modificationChanged(bool)));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateProperties()));
    setModified(false);
}

qreal Document::groundRatioPower(int exponent)
{
    return qPow(GROUND_RATIO, exponent);
}

qreal Document::fontSize(int level, qreal scale)
{
    return baseFontSize * groundRatioPower(level) * scale;
}

bool Document::isHeading(const QTextBlock &block)
{
    int type = block.blockFormat().intProperty(ParagraphTypeId);
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
    newCounter(SectionCounter, None, tr("Section"), sectionFont, "", "");
    newCounter(SubsectionCounter, SectionCounter, tr("Subsection"), subsectionFont, "", "");
    newCounter(Equation, SectionCounter, tr("Equation"), textFont, "(", ")");
    newCounter(Figure, SectionCounter, tr("Figure"), textFont, "", ".");
    newCounter(Table, SectionCounter, tr("Table"), textFont, "", ".");
    newCounter(Axiom, SectionCounter, tr("Axiom"), textFontBold, tr("Axiom"), ".");
    newCounter(Definition, SectionCounter, tr("Definition"), textFontBold, tr("Definition"), ".");
    newCounter(Proposition, SectionCounter, tr("Proposition"), textFontBold, tr("Proposition"), ".");
    newCounter(Lemma, SectionCounter, tr("Lemma"), textFontBold, tr("Lemma"), ".");
    newCounter(Theorem, SectionCounter, tr("Theorem"), textFontBold, tr("Theorem"), ".");
    newCounter(Corollary, SectionCounter, tr("Corollary"), textFontBold, tr("Corollary"), ".");
    newCounter(Algorithm, SectionCounter, tr("Algorithm"), textFontBold, tr("Algorithm"), ".");
    newCounter(Problem, SectionCounter, tr("Problem"), textFontBold, tr("Problem"), ".");
    newCounter(Remark, SectionCounter, tr("Remark"), textFontItalic, tr("Remark"), ".");
    newCounter(Note, SectionCounter, tr("Note"), textFontItalic, tr("Note"), ".");
    newCounter(Example, SectionCounter, tr("Example"), textFontItalic, tr("Example"), ".");
    newCounter(Exercise, SectionCounter, tr("Exercise"), textFontItalic, tr("Exercise"), ".");
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
    case SubsectionCounter:
        list.prepend(QString::number(counters[SubsectionCounter].count));
    case SectionCounter:
        list.prepend(QString::number(counters[SectionCounter].count));
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
    QTextBlockFormat blockFormat = cursor.blockFormat();
    blockFormat.setProperty(Document::ParagraphTypeId, Document::TextBody);
    blockFormat.setTextIndent(0.0);
    if (blockFormat.hasProperty(Document::CounterTypeId))
        blockFormat.setProperty(Document::CounterTypeId, Document::None);
    blockFormat.setProperty(Document::ParagraphTypeId, Document::TextBody);
    cursor.mergeBlockFormat(blockFormat);
    QTextCharFormat format;
    format.setFont(textFont);
    applyFormatToBlock(cursor, format, true);
}

void Document::insertTitleFrame()
{
    if (titleFrame != nullptr)
        return;
    QTextCursor cursor(this);
    cursor.movePosition(QTextCursor::Start);
    QTextFrameFormat frameFormat;
    titleFrame = cursor.insertFrame(frameFormat);
    QTextCharFormat format;
    QTextBlockFormat blockFormat = cursor.blockFormat();
    format.setFont(titleFont);
    blockFormat.setAlignment(Qt::AlignCenter);
    cursor.setBlockFormat(blockFormat);
    cursor.setBlockCharFormat(format);
}

void Document::addAuthor()
{
    if (!hasTitleFrame())
        return;
    if (numberOfAuthors() == 0)
    {
        QTextTableFormat tableFormat;
        tableFormat.setTopMargin(12.0);
        tableFormat.setAlignment(Qt::AlignCenter);
        authorsTable = titleFrame->lastCursorPosition().insertTable(1,1);
    }
    else
    {
        authorsTable->appendColumns(1);
        QTextTableFormat tableFormat = authorsTable->format();
        int n = numberOfAuthors();
        tableFormat.setColumnWidthConstraints(
                    QVector<QTextLength>(n, QTextLength(QTextLength::PercentageLength, 100.0/n)));
    }
}

int Document::numberOfAuthors()
{
    if (authorsTable == nullptr)
        return 0;
    return authorsTable->columns();
}

Document::Author Document::nthAuthor(int n)
{
    Author author;
    int N = numberOfAuthors();
    if (n >= 0 && n < N)
    {
        QTextTableCell cell = authorsTable->cellAt(0, n);
        QTextFrame::iterator it;
        QStringList names;
        for (it = cell.begin(); !it.atEnd(); ++it)
        {
            QTextFrame *childFrame = it.currentFrame();
            QTextBlock childBlock = it.currentBlock();
            if (childFrame)
            {
                if (childFrame->frameFormat().intProperty(FrameTypeId) == AffiliationFrame)
                    author.affiliation.append(getFrameLines(childFrame));
                if (childFrame->frameFormat().intProperty(FrameTypeId) == EmailFrame)
                    author.emails.append(getFrameLines(childFrame));
            }
            else if (childBlock.isValid())
            {
                names.append(childBlock.text().trimmed());
            }
        }
        author.name = names.join(" ");
    }
    return author;
}

QList<QString> Document::getFrameLines(QTextFrame *frame)
{
    QList<QString> list;
    QTextFrame::iterator it;
    for (it = frame->begin(); !it.atEnd(); ++it)
    {
        QTextBlock block = it.currentBlock();
        if (block.isValid())
            list.append(block.text().trimmed());
    }
    return list;
}

void Document::applyFormatToBlock(const QTextCursor &originalCursor, const QTextCharFormat &format, bool fromStart)
{
    QTextCursor cursor(originalCursor);
    if (fromStart)
    {
        cursor.setBlockCharFormat(format);
        cursor.movePosition(QTextCursor::StartOfBlock);
    }
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    if (cursor.hasSelection())
        cursor.mergeCharFormat(format);
}

const QTextCursor Document::firstTitleCursorPosition()
{
   if (hasTitleFrame())
       return titleFrame->firstCursorPosition();
   return QTextCursor();
}

QString Document::getCurrentTitle()
{
    if (hasTitleFrame())
    {
        QTextCursor cursor(titleFrame->firstCursorPosition());
        if (cursor.currentFrame() == titleFrame)
        {
            QTextBlock block = cursor.block();
            return block.text();
        }
    }
    return QString(tr("Untitled"));
}

void Document::on_modificationChanged(bool changed)
{
    if (!changed)
        return;
    timer->start(UPDATE_ON_MODIFICATION_TIMEOUT);
    setModified(false);
}

void Document::updateProperties()
{
    QString title = getCurrentTitle();
    if (title != lastTitle)
        emit titleChanged(title);
    lastTitle = title;
}
