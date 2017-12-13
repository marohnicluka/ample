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
#include "tablepropertiesdialog.h"

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

    paragraphStyleToolButton = new QToolButton(this);
    QMenu *paragraphStyleMenu = new QMenu(this);
    paragraphStyleMenu->addAction(ui->actionInsertSection);
    paragraphStyleMenu->addAction(ui->actionInsertSubsection);
    paragraphStyleMenu->addAction(ui->actionInsertSubsubsection);
    ui->actionParagraphStyle->setMenu(paragraphStyleMenu);
    paragraphStyleToolButton->setDefaultAction(ui->actionParagraphStyle);
    paragraphStyleToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->textToolBar->insertWidget(ui->actionTextBold,paragraphStyleToolButton);

    activeDocumentsToolButton = new QToolButton(this);
    activeDocumentsMenu = new QMenu(this);
    activeDocumentsGroup = new QActionGroup(this);
    activeDocumentsGroup->setExclusive(true);
    ui->actionSwitchDocuments->setMenu(activeDocumentsMenu);
    activeDocumentsToolButton->setDefaultAction(ui->actionSwitchDocuments);
    activeDocumentsToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->insertWidget(ui->actionDocumentProperties,activeDocumentsToolButton);

    recentDocumentsToolButton = new QToolButton(this);
    recentDocumentsMenu = new QMenu(this);
    recentDocumentsGroup = new QActionGroup(this);
    ui->actionOpenRecent->setMenu(recentDocumentsMenu);
    recentDocumentsToolButton->setDefaultAction(ui->actionOpenRecent);
    recentDocumentsToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->insertWidget(ui->actionNewDocument,recentDocumentsToolButton);

    QList<int> splitterSizes;
    splitterSizes << 183 << 747;
    ui->splitter->setSizes(splitterSizes);

    loadFonts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

TextEditor *MainWindow::currentTextEditor()
{
    return (TextEditor*)ui->documentView->currentWidget();
}

void MainWindow::loadFonts()
{
    QStringList fonts, failed;
    fonts << "FreeMono.ttf" << "FreeMonoBold.ttf" << "FreeMonoBoldOblique.ttf" << "FreeMonoOblique.ttf"
          << "FreeSans.ttf" << "FreeSansBold.ttf" << "FreeSansBoldOblique.ttf" << "FreeSansOblique.ttf"
          << "FreeSerif.ttf" << "FreeSerifBold.ttf" << "FreeSerifBoldItalic.ttf" << "FreeSerifItalic.ttf"
          << "LiberationSans-Bold.ttf" << "LiberationSans-BoldItalic.ttf" << "LiberationSans-Italic.ttf"
          << "LiberationSans-Regular.ttf";
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
    Worksheet *doc = new Worksheet(&ct, this);
    TextEditor *editor = new TextEditor(doc, this);
    connect(editor, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(editor, SIGNAL(undoAvailable(bool)), ui->actionUndo, SLOT(setEnabled(bool)));
    connect(editor, SIGNAL(redoAvailable(bool)), ui->actionRedo, SLOT(setEnabled(bool)));
#ifndef QT_NO_CLIPBOARD
    connect(editor, SIGNAL(copyAvailable(bool)), this, SLOT(copyAvailableChanged(bool)));
#endif
    int index = ui->documentView->addWidget(editor);
    ui->documentView->setCurrentIndex(index);
    QAction *action = editor->createMenuAction(index, activeDocumentsGroup);
    activeDocumentsMenu->addAction(action);
    connect(editor, SIGNAL(focusRequested(int)), this, SLOT(currentDocumentChanged(int)));
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
}

void MainWindow::currentDocumentChanged(int index)
{
    ui->documentView->setCurrentIndex(index);
}

void MainWindow::currentDocumentTitleChanged(const QString &newTitle)
{
    this->setWindowTitle(newTitle);
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
    QTextCharFormat format;
    bool isMath = ui->actionTextMath->isChecked();
    format.setFontFamily(isMath ? "FreeMono" : "FreeSerif");
    mergeFormatOnWordOrSelection(format);
}

void MainWindow::on_documentView_currentChanged(int index)
{
    TextEditor *editor = (TextEditor*)ui->documentView->widget(index);
    currentDocument = editor->worksheet();
}

void MainWindow::on_actionInsertSection_triggered()
{
    QTextCursor cursor = currentTextEditor()->textCursor();
    currentDocument->insertHeadingFrame(cursor, 1);
}

void MainWindow::on_actionInsertSubsection_triggered()
{
    QTextCursor cursor = currentTextEditor()->textCursor();
    currentDocument->insertHeadingFrame(cursor, 2);
}

void MainWindow::on_actionInsertSubsubsection_triggered()
{
    QTextCursor cursor = currentTextEditor()->textCursor();
    currentDocument->insertHeadingFrame(cursor, 3);
}

void MainWindow::on_actionInsertCAS_triggered()
{
    QTextCursor cursor = currentTextEditor()->textCursor();
    currentDocument->insertCasInputFrame(cursor);
}

void MainWindow::on_actionInsertTable_triggered()
{
    TablePropertiesDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QTextCursor cursor = currentTextEditor()->textCursor();
        int flags = dialog.tableFlags();
        int rows = dialog.tableRows(), columns = dialog.tableColumns();
        int headerRowCount = dialog.tableHeaderRows();
        currentDocument->insertTable(cursor, rows, columns, headerRowCount, flags);
    }
}
