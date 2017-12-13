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

#include <QTextTableCell>
#include <QTextTableFormat>
#include <QTextLength>
#include <QDebug>
#include "worksheet.h"
#include "glyphs.h"

using namespace giac;

Worksheet::Worksheet(GIAC_CONTEXT, QObject *parent) : QTextDocument(parent)
{
    gcontext = contextptr;
    ghighlighter = new GiacHighlighter(this);
    setModified(false);
    connect(this, SIGNAL(modificationChanged(bool)), this, SLOT(on_modificationChanged(bool)));
}

QString Worksheet::frameText(QTextFrame *frame)
{
    QStringList lines;
    QTextFrame::iterator it;
    for (it = frame->begin(); !it.atEnd(); ++it)
    {
        if (it.currentFrame())
            continue;
        QTextBlock block = it.currentBlock();
        if (block.isValid())
            lines.append(block.text());
    }
    return lines.join("\n");
}

void Worksheet::insertHeadingFrame(QTextCursor &cursor, int level)
{
    if (cursor.currentFrame() != rootFrame())
        return;
    QTextFrameFormat frameFormat;
    frameFormat.setProperty(Subtype, Heading);
    frameFormat.setProperty(Level, level);
    QTextFrame *frame = cursor.insertFrame(frameFormat);
    QTextCharFormat format;
    format.setFontFamily("LiberationSans");
    QList<int> fontSizes = QList<int>() << 24 << 17 << 12;
    format.setFontPointSize(fontSizes.at(level - 1));
    format.setFontWeight(QFont::Bold);
    cursor.setCharFormat(format);
    cursor.setBlockCharFormat(format);
    connect(frame, SIGNAL(destroyed(QObject*)), SLOT(updateEnumeration(QObject*)));
    updateEnumeration();
}

void Worksheet::insertCasInputFrame(QTextCursor &cursor)
{
    if (cursor.currentFrame() != rootFrame())
        return;
    QTextFrameFormat frameFormat;
    QTextCharFormat format;
    format.setFontFamily("FreeMono");
    format.setFontPointSize(12);
    int margin = QFontMetrics(format.font()).width(">") * 3;
    frameFormat.setProperty(Subtype, CasInput);
    frameFormat.setLeftMargin(margin);
    QTextFrame *frame = cursor.insertFrame(frameFormat);
    cursor.setCharFormat(format);
    cursor.setBlockCharFormat(format);
    connect(frame, SIGNAL(destroyed(QObject*)), SLOT(casInputDestroyed(QObject*)));
}

QTextFrame* Worksheet::insertCasOutputFrame(QTextFrame *inputFrame)
{
    QTextFrameFormat inputFrameFormat = inputFrame->frameFormat();
    if (inputFrameFormat.hasProperty(AssociatedFrame))
        return 0;
    QTextFrameFormat outputFrameFormat;
    outputFrameFormat.setProperty(Subtype, CasOutput);
    outputFrameFormat.setProperty(AssociatedFrame, qVariantFromValue((void*)inputFrame));
    outputFrameFormat.setProperty(Editable, false);
    QTextCursor cursor(inputFrame->lastCursorPosition());
    cursor.movePosition(QTextCursor::NextBlock);
    QTextFrame *outputFrame = cursor.insertFrame(outputFrameFormat);
    QTextCharFormat format;
    format.setFontFamily("FreeSans");
    format.setForeground(QBrush(Qt::darkGreen));
    format.setFontPointSize(10);
    cursor.setCharFormat(format);
    cursor.setBlockCharFormat(format);
    connect(outputFrame, SIGNAL(destroyed(QObject*)), this, SLOT(casOutputDestroyed(QObject*)));
    return outputFrame;
}

void Worksheet::removeFrame(QTextFrame *frame)
{
    frame->deleteLater();
}

void Worksheet::casInputDestroyed(QObject *casInput)
{
    QTextFrame *frame = (QTextFrame*)casInput;
    QTextFrameFormat frameFormat = frame->frameFormat();
    QString text = frameText(frame);
    qDebug() << text;
    if (frameFormat.hasProperty(AssociatedFrame))
    {
        QTextFrame *outputFrame = (QTextFrame*)(frameFormat.property(AssociatedFrame).data());
        removeFrame(outputFrame);
    }
}

void Worksheet::casOutputDestroyed(QObject *casOutput)
{
    QTextFrame *frame = (QTextFrame*)casOutput;
    QTextFrameFormat frameFormat = frame->frameFormat();
    QTextFrame *inputFrame = (QTextFrame*)(frameFormat.property(AssociatedFrame).data());
    frameFormat = inputFrame->frameFormat();
    frameFormat.clearProperty(AssociatedFrame);
    inputFrame->setFrameFormat(frameFormat);
}

void Worksheet::updateEnumeration(QObject *deletedObject)
{
    int counter[3] = { 0, 0, 0 };
    QTextFrame::iterator it;
    QTextFrame *deletedFrame = (deletedObject == nullptr ?
                                    nullptr : (QTextFrame*)deletedObject);
    for (it = rootFrame()->begin(); !it.atEnd(); ++it)
    {
        QTextFrame *frame = it.currentFrame();
        QTextFrameFormat frameFormat;
        if (frame && frame != deletedFrame &&
                (frameFormat = frame->frameFormat()).intProperty(Subtype) == Heading)
        {
            int level = frameFormat.intProperty(Level);
            ++counter[level - 1];
            if (level < 3)
                counter[2] = 0;
            if (level == 1)
                counter[1] = 0;
            QStringList list;
            for (int i = 0; i < level; ++i)
                list.append(QString::number(counter[i]));
            QString number = list.join(".");
            frameFormat.setProperty(Label, number);
            frame->setFrameFormat(frameFormat);
            QTextCursor cursor(frame->firstCursorPosition());
            QTextBlockFormat blockFormat = cursor.blockFormat();
            number += Glyphs::emQuad();
            qreal indent = QFontMetrics(cursor.blockCharFormat().font()).width(number);
            blockFormat.setTextIndent(indent);
            cursor.setBlockFormat(blockFormat);
        }
    }
}

void Worksheet::insertTable(QTextCursor &cursor, int rows, int columns, int headerRowCount, int flags)
{
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignCenter);
    tableFormat.setCellPadding(6);
    tableFormat.setCellSpacing(0);
    tableFormat.setBorder(2.0);
    tableFormat.setBorderBrush(QBrush(Qt::gray));
    tableFormat.setProperty(Flags, flags);
    tableFormat.setHeaderRowCount(headerRowCount);
    QTextTable *table = cursor.insertTable(
                rows, columns, tableFormat);
    for (int i = headerRowCount; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            QTextTableCell cell = table->cellAt(i, j);
            QTextCharFormat cellFormat = cell.format();
            cellFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);
            if ((flags & Worksheet::Numeric) != 0)
                cellFormat.setFontFamily("FreeSans");
            cell.setFormat(cellFormat);
        }
    }
}

bool Worksheet::isHeadingFrame(QTextFrame *frame, int &level)
{
    QTextFrameFormat format = frame->frameFormat();
    bool yes = format.hasProperty(Subtype) && format.intProperty(Subtype) == Heading;
    if (yes)
        level = format.intProperty(Level);
    return yes;
}

bool Worksheet::isCasInputFrame(QTextFrame *frame)
{
    QTextFrameFormat format = frame->frameFormat();
    return format.hasProperty(Subtype) && format.intProperty(Subtype) == CasInput;
}

bool Worksheet::isCasOutputFrame(QTextFrame *frame)
{
    QTextFrameFormat format = frame->frameFormat();
    return format.hasProperty(Subtype) && format.intProperty(Subtype) == CasOutput;
}

bool Worksheet::isTable(QTextFrame *frame, int &flags)
{
    bool yes = frame->format().isTableFormat();
    if (yes)
        flags = qobject_cast<QTextTable*>(frame)->format().intProperty(Flags);
    return yes;
}

void Worksheet::on_modificationChanged(bool changed)
{
}
