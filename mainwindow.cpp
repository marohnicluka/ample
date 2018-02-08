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

    activeDocumentsToolButton = new QToolButton(this);
    activeDocumentsMenu = new QMenu(this);
    activeDocumentsGroup = new QActionGroup(this);
    activeDocumentsGroup->setExclusive(true);
    ui->actionSwitchDocuments->setMenu(activeDocumentsMenu);
    activeDocumentsToolButton->setDefaultAction(ui->actionSwitchDocuments);
    activeDocumentsToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->insertWidget(ui->actionRecompute, activeDocumentsToolButton);

    recentDocumentsToolButton = new QToolButton(this);
    recentDocumentsMenu = new QMenu(this);
    recentDocumentsGroup = new QActionGroup(this);
    ui->actionOpenRecent->setMenu(recentDocumentsMenu);
    recentDocumentsToolButton->setDefaultAction(ui->actionOpenRecent);
    recentDocumentsToolButton->setPopupMode(QToolButton::InstantPopup);
    ui->mainToolBar->insertWidget(ui->actionNewDocument, recentDocumentsToolButton);

    fontFamilyChooser = new QFontComboBox(this);
    fontSizeChooser = new QSpinBox(this);
    fontChooser = new QWidget(this);
    fontChooserLayout = new QGridLayout(fontChooser);
    fontChooserLayout->addWidget(fontFamilyChooser, 0, 0, 1, 1);
    fontChooserLayout->addWidget(fontSizeChooser, 0, 1, 1, 1);
    fontChooserLayout->setHorizontalSpacing(6);
    fontChooserLayout->setMargin(0);
    ui->textToolBar->insertWidget(ui->actionTextBold, fontChooser);

    loadFonts();

    CommandIndex *commandIndex = new CommandIndex(this);

    CommandIndexDialog *commandIndexDialog = new CommandIndexDialog(commandIndex, this);

    commandIndexDialog->show();
    commandIndexDialog->raise();
    commandIndexDialog->activateWindow();

    session = new Session(this);
    connect(session, SIGNAL(processingStarted()), this, SLOT(giacProcessingStarted()));
    connect(session, SIGNAL(processingFinished(const gen &,const QStringList &)),
            this, SLOT(giacProcessingFinished(const gen &,const QStringList &)));
    ui->messagesTextBrowser->setFont(QFont("FreeSerif", 12));
    ui->messagesTextBrowser->setText(QString("<html><style>radicand{text-decoration:overline;}</style>") +
                                     "<body>Επιστρέφει το μιγαδικό αριθμό ίσο με ∣<i>AC</i>∣&sdot;∣<i>BD</i>∣&sdot;∣<i>AD</i>∣<sup>&minus;1</sup>∣<i>BC</i>∣<sup>&minus;1</sup>.</body></html>");
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::textAlignChanged(QAction *action)
{
    if (action == ui->actionAlignLeft) //setAlignment(Qt::AlignLeft | Qt::AlignAbsolute)
        ;
    else if (action == ui->actionAlignCenter) //setAlignment(Qt::AlignHCenter)
        ;
    else if (action == ui->actionAlignRight) //setAlignment(Qt::AlignRight | Qt::AlignAbsolute)
        ;
    else if (action == ui->actionAlignJustify) //setAlignment(Qt::AlignJustify)
        ;
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

void MainWindow::giacProcessingStarted()
{
    ui->messagesTextBrowser->setText("Processing...");
    ui->outputLineEdit->clear();
}

gen strToGiac(const QString &str)
{
    std::stringstream ss;
    gen g;
    ss << str.toStdString();
    ss >> g;
    return g;
}

QString giacToStr(const gen &g)
{
    std::stringstream ss;
    ss << g;
    return ss.str().data();
}

void MainWindow::giacProcessingFinished(const gen &g, const QStringList &messages)
{
    ui->outputLineEdit->setText(giacToStr(g));
    ui->messagesTextBrowser->setText(messages.join('\n'));
}

void MainWindow::on_evaluateButton_clicked()
{
    QString command = ui->inputLineEdit->text();
    session->evaluate(strToGiac(command));
}
