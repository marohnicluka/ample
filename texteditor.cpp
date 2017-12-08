#include <QTextLayout>
#include <QPainter>
#include <QScrollBar>
#include "texteditor.h"

TextEditor::TextEditor(Document *document, QWidget *parent) : QTextEdit(parent)
{
    setDocument(document);
    doc = document;
    setAcceptRichText(false);
    setFrameStyle(QFrame::NoFrame);
    lastBlockCount = 0;
}

void TextEditor::paintEvent(QPaintEvent *e)
{
    QTextEdit::paintEvent(e);
    QTextBlock block = doc->begin();
    while (block.isValid())
    {
        QTextLayout *layout = block.layout();
        QTextBlockFormat format = block.blockFormat();
        if (format.hasProperty(Document::CounterTypeId) &&
                format.intProperty(Document::CounterTypeId) != Document::None &&
                format.hasProperty(Document::CounterTagId))
        {
            QFont font = doc->counterFont(format.intProperty(Document::CounterTypeId));
            qreal x = layout->position().x() - Document::paragraphMargin;
            qreal y = layout->position().y();
            QFontMetrics fontMetrics(font);
            QString tag = format.stringProperty(Document::CounterTagId);
            int nLines = layout->lineCount();
            if (nLines == 0)
                y += fontMetrics.ascent();
            else
                y += layout->lineAt(0).ascent() + fontMetrics.leading();
            QPointF where(x,y);
            QPainter painter(this->viewport());
            painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
            painter.setFont(font);
            painter.drawText(where, tag);
        }
        block = block.next();
    }
}

void TextEditor::blockCountChanged(int count)
{
    QTextCursor cursor = textCursor();
    if (cursor.currentFrame() != doc->rootFrame())
        return;
    if (lastBlockCount > count && Document::isHeading(cursor.block()))
    {
        if (cursor.block().text().length() == 0)
            doc->blockToParagraph(cursor);
        else if (!cursor.atBlockEnd())
            Document::applyFormatToEndOfBlock(cursor);
    }
    else if (count == 1 + lastBlockCount && cursor.atBlockStart())
    {
        if (Document::isHeading(cursor.block().previous()))
            doc->blockToParagraph(cursor);
    }
    lastBlockCount = count;
    doc->updateEnumeration();
}

void TextEditor::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection() && !cursorAt(QTextCursor::EndOfWord))
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    mergeCurrentCharFormat(format);
}

void TextEditor::paragraphStyleChanged(int type)
{
    bool isList = type == Document::List || type == Document::NumberedList;
    QTextCursor cursor = textCursor();
    QTextBlockFormat blockFormat = cursor.blockFormat();
    if (cursor.currentFrame() != doc->rootFrame() ||
            blockFormat.intProperty(Document::ParagraphStyleId) == type)
        return;
    QTextCursor pCursor(cursor);
    QTextCharFormat charFormat;
    pCursor.movePosition(QTextCursor::StartOfBlock);
    pCursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
    bool countingChanged = false;
    switch (type)
    {
    case Document::TextBody:
        charFormat.setFont(doc->textFont);
        break;
    case Document::Title:
        charFormat.setFont(doc->titleFont);
        break;
    case Document::Section:
        charFormat.setFont(doc->sectionFont);
        blockFormat.setProperty(Document::CounterTypeId, Document::SectionCounterType);
        countingChanged = true;
        break;
    case Document::Subsection:
        charFormat.setFont(doc->subsectionFont);
        blockFormat.setProperty(Document::CounterTypeId, Document::SubsectionCounterType);
        countingChanged = true;
        break;
    default:
        break;
    }
    if (!isList)
    {
        cursor.mergeBlockCharFormat(charFormat);
        pCursor.mergeCharFormat(charFormat);
        blockFormat.setProperty(Document::ParagraphStyleId, type);
        cursor.setBlockFormat(blockFormat);
    }
    if (countingChanged)
        doc->updateEnumeration();
}

bool TextEditor::cursorAt(QTextCursor::MoveOperation op)
{
    QTextCursor cursor = textCursor();
    QTextCursor pCursor(cursor);
    pCursor.movePosition(op);
    return cursor.position() == pCursor.position();
}
