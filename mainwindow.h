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
#include "texteditor.h"

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
    Ui::MainWindow *ui;
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
    void textAlignChanged(QAction* action);
    void clipboardDataChanged();
    void copyAvailableChanged(bool yes);
};

#endif // MAINWINDOW_H
