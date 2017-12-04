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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>

using namespace giac;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QActionGroup *alignGroup = new QActionGroup(this);
    alignGroup->addAction(ui->actionAlignLeft);
    alignGroup->addAction(ui->actionAlignCenter);
    alignGroup->addAction(ui->actionAlignRight);
    alignGroup->addAction(ui->actionAlignJustify);
    alignGroup->setExclusive(true);
    connect(alignGroup, SIGNAL(triggered(QAction*)), this, SLOT(textAlign_changed(QAction*)));
    QActionGroup *paragraphGroup = new QActionGroup(this);
    paragraphGroup->addAction(ui->actionTextStyleParagraph);
    paragraphGroup->addAction(ui->actionTextStyleTitle);
    paragraphGroup->addAction(ui->actionTextStyleSection);
    paragraphGroup->addAction(ui->actionTextStyleSubsection);
    paragraphGroup->addAction(ui->actionTextStyleSubsubsection);
    paragraphGroup->addAction(ui->actionTextStyleListBullets);
    paragraphGroup->addAction(ui->actionTextStyleListSquares);
    paragraphGroup->addAction(ui->actionTextStyleNumberedList);
    paragraphGroup->addAction(ui->actionTextStyleOrderedListLetters);
    paragraphGroup->addAction(ui->actionTextStyleOrderedListCapitals);
    paragraphGroup->addAction(ui->actionTextStyleOrderedListRoman);
    paragraphGroup->addAction(ui->actionTextStyleOrderedListRomanSmall);
    paragraphGroup->setExclusive(true);
    connect(paragraphGroup, SIGNAL(triggered(QAction*)), this, SLOT(paragraphStyle_changed(QAction*)));
    paragraphStyleToolButton = new QToolButton(this);
    paragraphStyleToolButton->setDefaultAction(ui->actionParagraphStyle);
    QMenu *paragraphStyleMenu = new QMenu(this);
    paragraphStyleMenu->addActions(paragraphGroup->actions());
    paragraphStyleToolButton->setMenu(paragraphStyleMenu);
    paragraphStyleToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->textToolBar->insertWidget(ui->actionTextBold,paragraphStyleToolButton);
    QList<int> outlineSplitterSizes;
    outlineSplitterSizes << 100 << 450;
    ui->outlineSplitter->setSizes(outlineSplitterSizes);
    /*
    QTextCursor cursor(doc);
    QTextFrameFormat bodyFrameFormat;
    bodyFrameFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));
    bodyFrameFormat.setPadding(10);
    bodyFrameFormat.setBorder(0.0);
    cursor.insertFrame(bodyFrameFormat);
    QTextCharFormat fmt;
    fmt.setFontPointSize(11.0);
    fmt.setFontFamily("FreeMono");
    fmt.setFontWeight(QFont::Weight::Bold);
    cursor.mergeCharFormat(fmt);
    ui->textEdit->mergeCurrentCharFormat(fmt);
    */
}

void MainWindow::textAlign_changed(QAction *a) {
    if (a == ui->actionAlignLeft)
        qDebug("Left");
    //textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == ui->actionAlignCenter)
        qDebug("Center");
    //textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == ui->actionAlignRight)
        qDebug("Right");
    //textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == ui->actionAlignJustify)
        qDebug("Justified");
    //textEdit->setAlignment(Qt::AlignJustify);
}

void MainWindow::paragraphStyle_changed(QAction *a) {

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addNewDocument() {
    context ct;
    Document *doc = new Document(&ct, this);
    QTextEdit *editor = new QTextEdit(this);
    editor->setDocument(doc);
    editor->setFrameStyle(QFrame::NoFrame);
    int index = ui->editorTabs->addTab(editor,QString("Unnamed"));
    ui->editorTabs->setCurrentIndex(index);
}

void MainWindow::on_sidebarTabs_currentChanged(int index)
{
    ui->sidebarTabs->setDocumentMode(index == 0);
}

void MainWindow::on_editorTabs_currentChanged(int index)
{
    QTextEdit *editor = qobject_cast<QTextEdit*>(ui->editorTabs->widget(index));
    currentDocument = qobject_cast<Document*>(editor->document());
}

void MainWindow::on_editorTabs_tabCloseRequested(int index)
{

}

void MainWindow::on_actionNewDocument_triggered()
{
    addNewDocument();
}
