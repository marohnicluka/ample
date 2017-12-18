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

#include <QTextLayout>
#include <QPainter>
#include <QScrollBar>
#include <QApplication>
#include <QString>
#include <QFileInfo>
#include <QTextDocumentFragment>
#include "texteditor.h"

int TextEditor::unnamedCount = 0;

TextEditor::TextEditor(Worksheet *worksheet, QWidget *parent)
    : QTextEdit(parent)
{
    setDocument(worksheet);
    m_worksheet = worksheet;
    //setAcceptRichText(false);
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(cursorMoved()));
}

TextEditor::~TextEditor()
{
    activeDocuments->removeAction(menuAction);
}

QAction* TextEditor::createMenuAction(int index, QActionGroup *actionGroup)
{
    menuAction = new QAction(this);
    menuAction->setData(index);
    menuAction->setCheckable(true);
    connect(menuAction, SIGNAL(triggered(bool)), this, SLOT(menuActionTriggered(bool)));
    QString text;
    if (worksheet()->isUnnamed())
        text = tr("Unnamed document ") + QString::number(++unnamedCount);
    else
        text = QFileInfo(worksheet()->fileName()).baseName();
    menuAction->setText(text);
    activeDocuments = actionGroup;
    activeDocuments->addAction(menuAction);
    return menuAction;
}

void TextEditor::paintEvent(QPaintEvent *event)
{
    QTextEdit::paintEvent(event);
    QPainter painter(this->viewport());
    painter.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
    QTextFrame::iterator it;
    int level;
    for (it = worksheet()->rootFrame()->begin(); !it.atEnd(); ++it)
    {
        QTextFrame *frame = it.currentFrame();
        if (frame == 0)
            continue;
        if (worksheet()->isHeadingFrame(frame, level))
        {

            QTextBlock block = frame->firstCursorPosition().block();
            QFont font = block.charFormat().font();
            QTextLayout *layout = block.layout();
            int headingHeight = layout->lineAt(0).ascent() + 1;
            block = block.previous();
            if (!block.isValid())
                continue;
            layout = block.layout();
            int previousHeight = layout->boundingRect().height();
            QFontMetrics fontMetrics(font);
            qreal x = layout->position().x();
            qreal y = layout->position().y() + fontMetrics.leading() + previousHeight + headingHeight;
            QString label = frame->frameFormat().stringProperty(Worksheet::Label);
            QPointF location(x,y);
            painter.setFont(font);
            painter.drawText(location, label);
        }
    }
}

void TextEditor::keyPressEvent(QKeyEvent *event)
{
    /*
    if (!event->matches(QKeySequence::Copy) && event->text().length() > 0)
        return;
    */
    QTextCursor cursor = textCursor();
    bool isShiftPressed = event->modifiers().testFlag(Qt::ShiftModifier);
    //bool isCtrlPressed = event->modifiers().testFlag(Qt::ControlModifier);
    //bool isAltPressed = event->modifiers().testFlag(Qt::AltModifier);
    QTextEdit::keyPressEvent(event);
}

void TextEditor::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection() && !cursorAtEndOfWord())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    mergeCurrentCharFormat(format);
}

void TextEditor::cursorMoved()
{
}

bool TextEditor::cursorAtEndOfWord()
{
    QTextCursor cursor = textCursor();
    QTextCursor pCursor(cursor);
    pCursor.movePosition(QTextCursor::EndOfWord);
    return cursor.position() == pCursor.position();
}

void TextEditor::menuActionTriggered(bool active)
{
    if (active)
        emit focusRequested(menuAction->data().toInt());
}
