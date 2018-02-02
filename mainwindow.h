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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QAction>
#include <QFontComboBox>
#include <QSpinBox>
#include <QGridLayout>
#include "texteditor.h"
#include "mathdisplaywidget.h"
#include "session.h"
#include "commandindex.h"
#include "commandindexdialog.h"
#include <sstream>

namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Session *session;
    Ui::MainWindow *ui;
    QFontComboBox *fontFamilyChooser;
    QSpinBox *fontSizeChooser;
    QGridLayout *fontChooserLayout;
    QWidget *fontChooser;
    QToolButton *activeDocumentsToolButton;
    QToolButton *recentDocumentsToolButton;
    QMenu *activeDocumentsMenu;
    QMenu *recentDocumentsMenu;
    QActionGroup *activeDocumentsGroup;
    QActionGroup *recentDocumentsGroup;
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    bool cursorAt(QTextCursor::MoveOperation op);
    void loadFonts();

private slots:
    void giacProcessingStarted();
    void giacProcessingFinished(const gen &g, const QStringList &messages);
    void textAlignChanged(QAction* action);
    void clipboardDataChanged();
    void copyAvailableChanged(bool yes);
    void on_evaluateButton_clicked();
};

#endif // MAINWINDOW_H
