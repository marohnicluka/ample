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

    switchDocumentsToolButton = new QToolButton(this);
    switchDocumentsMenu = new QMenu(this);
    ui->actionSwitchDocuments->setMenu(switchDocumentsMenu);
    switchDocumentsToolButton->setDefaultAction(ui->actionSwitchDocuments);
    switchDocumentsToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->insertWidget(ui->actionDocumentProperties,switchDocumentsToolButton);

    recentDocumentsToolButton = new QToolButton(this);
    recentDocumentsMenu = new QMenu(this);
    ui->actionOpenRecent->setMenu(recentDocumentsMenu);
    recentDocumentsToolButton->setDefaultAction(ui->actionOpenRecent);
    recentDocumentsToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->insertWidget(ui->actionNewDocument,recentDocumentsToolButton);

    QList<int> outlineSplitterSizes;
    outlineSplitterSizes << 100 << 450;
    ui->outlineSplitter->setSizes(outlineSplitterSizes);

    loadFonts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

TextEditor *MainWindow::currentTextEditor()
{
    TextEditor *editor = (TextEditor*)ui->editorTabs->currentWidget();
    assert(editor != nullptr);
    return editor;
}

void MainWindow::loadFonts()
{
    QStringList fonts, failed;
    fonts << "FreeMono.ttf" << "FreeMonoBold.ttf" << "FreeMonoBoldOblique.ttf" << "FreeMonoOblique.ttf"
          << "FreeSans.ttf" << "FreeSansBold.ttf" << "FreeSansBoldOblique.ttf" << "FreeSansOblique.ttf"
          << "FreeSerif.ttf" << "FreeSerifBold.ttf" << "FreeSerifBoldItalic.ttf" << "FreeSerifItalic.ttf"
          << "LiberationSans-Bold.ttf" << "LiberationSans-BoldItalic.ttf"
          << "LiberationSans-Italic.ttf" << "LiberationSans-Regular.ttf";
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
    TextEditor *editor = new TextEditor(doc, this);
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
    currentTextEditor()->paragraphStyleChanged(type);
}

void MainWindow::blockCountChanged(int count) {
    currentTextEditor()->blockCountChanged(count);
}

void MainWindow::textAlignChanged(QAction *a)
{
    if (a == ui->actionAlignLeft)
        currentTextEditor()->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == ui->actionAlignCenter)
        currentTextEditor()->setAlignment(Qt::AlignHCenter);
    else if (a == ui->actionAlignRight)
        currentTextEditor()->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == ui->actionAlignJustify)
        currentTextEditor()->setAlignment(Qt::AlignJustify);
}

void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    currentTextEditor()->mergeFormatOnWordOrSelection(format);
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
    currentDocument = qobject_cast<Document*>(currentTextEditor()->document());
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
    currentTextEditor()->copy();
}

void MainWindow::on_actionCut_triggered()
{
    currentTextEditor()->cut();
}

void MainWindow::on_actionPaste_triggered()
{
    currentTextEditor()->paste();
}

void MainWindow::on_actionUndo_triggered()
{
    currentTextEditor()->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    currentTextEditor()->redo();
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
