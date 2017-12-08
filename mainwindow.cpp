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

#include <QMenu>
#include <QClipboard>
#include <QMimeData>
#include <QFontDatabase>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <qmath.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace giac;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        ui->actionPaste->setEnabled(md->hasText());
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
#endif
#ifdef QT_NO_CLIPBOARD
    ui->actionPaste->setEnabled(false);
#endif
    ui->actionCopy->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    QActionGroup *alignGroup = new QActionGroup(this);
    alignGroup->addAction(ui->actionAlignLeft);
    alignGroup->addAction(ui->actionAlignCenter);
    alignGroup->addAction(ui->actionAlignRight);
    alignGroup->addAction(ui->actionAlignJustify);
    alignGroup->setExclusive(true);
    connect(alignGroup, SIGNAL(triggered(QAction*)), this, SLOT(textAlignChanged(QAction*)));
    QActionGroup *paragraphGroup = new QActionGroup(this);
    paragraphGroup->addAction(ui->actionParagraphStyleTitle);
    paragraphGroup->addAction(ui->actionParagraphStyleSection);
    paragraphGroup->addAction(ui->actionParagraphStyleSubsection);
    paragraphGroup->addAction(ui->actionParagraphStyleTextBody);
    paragraphGroup->addAction(ui->actionParagraphStyleList);
    paragraphGroup->addAction(ui->actionParagraphStyleNumberedList);
    paragraphGroup->setExclusive(true);
    connect(paragraphGroup, SIGNAL(triggered(QAction*)), this, SLOT(paragraphStyleChanged(QAction*)));
    paragraphStyleToolButton = new QToolButton(this);
    QMenu *paragraphStyleMenu = new QMenu(this);
    paragraphStyleMenu->addActions(paragraphGroup->actions());
    ui->actionParagraphStyle->setMenu(paragraphStyleMenu);
    paragraphStyleToolButton->setDefaultAction(ui->actionParagraphStyle);
    paragraphStyleToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->textToolBar->insertWidget(ui->actionTextBold,paragraphStyleToolButton);
    QList<int> outlineSplitterSizes;
    outlineSplitterSizes << 100 << 450;
    ui->outlineSplitter->setSizes(outlineSplitterSizes);

    loadFonts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QTextEdit* MainWindow::currentTextEdit() {
    return qobject_cast<QTextEdit*>(ui->editorTabs->currentWidget());
}

void MainWindow::loadFonts()
{
    QStringList fonts, failed;
    fonts << "FreeMono.ttf" << "FreeMonoBold.ttf" << "FreeMonoBoldOblique.ttf" << "FreeMonoOblique.ttf"
          << "FreeSans.ttf" << "FreeSansBold.ttf" << "FreeSansBoldOblique.ttf" << "FreeSansOblique.ttf"
          << "FreeSerif.ttf" << "FreeSerifBold.ttf" << "FreeSerifBoldItalic.ttf" << "FreeSerifItalic.ttf";
    foreach (const QString fontName, fonts)
    {
        QFile res(":/fonts/" + fontName);
        if (!res.open(QIODevice::ReadOnly) || QFontDatabase::addApplicationFontFromData(res.readAll()) == -1)
            failed.append(fontName);
    }
    if (!failed.empty())
    {
        QString message = tr("Failed to load font(s): ") + failed.join(", ");
        QMessageBox::warning(this, tr("Font Loading Error"), message);
    }
}

void MainWindow::addNewDocument()
{
    context ct;
    Document *doc = new Document(&ct, this);
    QTextFrameFormat rootFrameFormat = doc->rootFrame()->frameFormat();
    rootFrameFormat.setLeftMargin(9);
    rootFrameFormat.setRightMargin(9);
    doc->rootFrame()->setFrameFormat(rootFrameFormat);
    QTextEdit *editor = new QTextEdit(this);
    editor->setAcceptRichText(false);
    editor->setDocument(doc);
    editor->setFrameStyle(QFrame::NoFrame);
    connect(editor, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(editor, SIGNAL(undoAvailable(bool)), ui->actionUndo, SLOT(setEnabled(bool)));
    connect(editor, SIGNAL(redoAvailable(bool)), ui->actionRedo, SLOT(setEnabled(bool)));
#ifndef QT_NO_CLIPBOARD
    connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(copyAvailableChanged(bool)));
#endif
    int index = ui->editorTabs->addTab(editor, QString("Unnamed"));
    ui->editorTabs->setCurrentIndex(index);
    connect(doc, SIGNAL(blockCountChanged(int)), this, SLOT(blockCountChanged(int)));
}

void MainWindow::updateTextStyleActions(const QFont &font)
{
    bool isCode = font.family() == currentDocument->codeFont.family();
    ui->actionTextBold->setChecked(!isCode && font.bold());
    ui->actionTextItalic->setChecked(!isCode && font.italic());
    ui->actionTextMath->setChecked(isCode);
}

void MainWindow::paragraphStyleChanged(QAction *a)
{
    Document::ParagraphType type;
    if (a == ui->actionParagraphStyleTextBody)
        type = Document::TextBody;
    else if (a == ui->actionParagraphStyleTitle)
        type = Document::Title;
    else if (a == ui->actionParagraphStyleSection)
        type = Document::Section;
    else if (a == ui->actionParagraphStyleSubsection)
        type = Document::Subsection;
    else if (a == ui->actionParagraphStyleList)
        type = Document::List;
    else if (a == ui->actionParagraphStyleNumberedList)
        type = Document::NumberedList;
    else
        return;
    bool isList = type == Document::List || type == Document::NumberedList;
    if (currentTextEdit() == nullptr)
        return;
    QTextCursor cursor = currentTextEdit()->textCursor();
    QTextBlockFormat blockFormat = cursor.blockFormat();
    if (cursor.currentFrame() != currentDocument->rootFrame() ||
            blockFormat.intProperty(Document::ParagraphStyleId) == type)
        return;
    QTextCursor pCursor(cursor);
    QTextCharFormat charFormat;
    pCursor.movePosition(QTextCursor::StartOfBlock);
    pCursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
    switch (type)
    {
    case Document::TextBody:
        charFormat.setFont(currentDocument->textFont);
        break;
    case Document::Title:
        charFormat.setFont(currentDocument->titleFont);
        break;
    case Document::Section:
        charFormat.setFont(currentDocument->sectionFont);
        break;
    case Document::Subsection:
        charFormat.setFont(currentDocument->subsectionFont);
        break;
    default:
        break;
    }
    if (!isList)
    {
        cursor.mergeBlockCharFormat(charFormat);
        pCursor.mergeCharFormat(charFormat);
        QTextBlockFormat blockFormat = cursor.blockFormat();
        blockFormat.setProperty(Document::ParagraphStyleId, type);
        cursor.setBlockFormat(blockFormat);
    }
}

void MainWindow::blockCountChanged(int count) {
    int lastCount = currentBlockCount;
    currentBlockCount = count;
    QTextCursor cursor = currentTextEdit()->textCursor();
    if (cursor.currentFrame() != currentDocument->rootFrame())
        return;
    bool atBlockStart = cursor.positionInBlock() == 0;
    QTextCursor pCursor(cursor);
    QTextCharFormat fmt;
    if (lastCount > count && !atBlockStart && Document::isHeading(cursor.block()))
    {
        fmt = cursor.charFormat();
        pCursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        pCursor.mergeCharFormat(fmt);
    }
    else if (count == 1 + lastCount && atBlockStart)
    {
        if (Document::isHeading(cursor.block().previous()))
        {
            fmt.setFont(currentDocument->textFont);
            pCursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::KeepAnchor);
            if (pCursor.hasSelection())
                pCursor.mergeCharFormat(fmt);
            cursor.mergeBlockCharFormat(fmt);
            QTextBlockFormat bfmt = cursor.blockFormat();
            bfmt.setProperty(Document::ParagraphStyleId, Document::TextBody);
            cursor.mergeBlockFormat(bfmt);
        }
    }
}

void MainWindow::textAlignChanged(QAction *a)
{
    if (a == ui->actionAlignLeft)
        currentTextEdit()->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == ui->actionAlignCenter)
        currentTextEdit()->setAlignment(Qt::AlignHCenter);
    else if (a == ui->actionAlignRight)
        currentTextEdit()->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == ui->actionAlignJustify)
        currentTextEdit()->setAlignment(Qt::AlignJustify);
}

bool MainWindow::cursorAt(QTextCursor::MoveOperation op)
{
    QTextCursor cursor = currentTextEdit()->textCursor();
    QTextCursor pCursor(cursor);
    pCursor.movePosition(op);
    return cursor.position() == pCursor.position();
}

void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextEdit *textEdit = currentTextEdit();
    if (textEdit == nullptr)
        return;
    QTextCursor cursor = textEdit->textCursor();
    if (!cursor.hasSelection() && !cursorAt(QTextCursor::EndOfWord))
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        ui->actionPaste->setEnabled(md->hasText());
#endif
}

void MainWindow::copyAvailableChanged(bool yes) {
    ui->actionCopy->setEnabled(yes);
    ui->actionCut->setEnabled(yes);
}

void MainWindow::currentCharFormatChanged(const QTextCharFormat &format)
{
    updateTextStyleActions(format.font());
}

void MainWindow::on_sidebarTabs_currentChanged(int index)
{
    ui->sidebarTabs->setDocumentMode(index == 0);
}

void MainWindow::on_editorTabs_currentChanged(int index)
{
    QTextEdit *editor = qobject_cast<QTextEdit*>(ui->editorTabs->widget(index));
    currentDocument = qobject_cast<Document*>(editor->document());
    currentBlockCount = currentDocument->blockCount();
}

void MainWindow::on_editorTabs_tabCloseRequested(int index)
{

}

void MainWindow::on_actionNewDocument_triggered()
{
    addNewDocument();
}

void MainWindow::on_actionCopy_triggered()
{
    currentTextEdit()->copy();
}

void MainWindow::on_actionCut_triggered()
{
    currentTextEdit()->cut();
}

void MainWindow::on_actionPaste_triggered()
{
    currentTextEdit()->paste();
}

void MainWindow::on_actionUndo_triggered()
{
    currentTextEdit()->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    currentTextEdit()->redo();
}
void MainWindow::on_actionTextBold_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::on_actionTextItalic_triggered()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::on_actionTextMath_triggered()
{
    QTextCharFormat fmt;
    bool isMath = ui->actionTextMath->isChecked();
    fmt.setFont(isMath ? currentDocument->codeFont : currentDocument->textFont);
    fmt.setProperty(Document::TextStyleId, isMath ? Document::MathTextStyle : Document::NormalTextStyle);
    mergeFormatOnWordOrSelection(fmt);
}
