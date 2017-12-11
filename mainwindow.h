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
#include "texteditor.h"
#include <giac/giac.h>

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
    Document *currentDocument;
    QToolButton *paragraphStyleToolButton;
    QToolButton *activeDocumentsToolButton;
    QToolButton *recentDocumentsToolButton;
    QMenu *activeDocumentsMenu;
    QMenu *recentDocumentsMenu;
    QActionGroup *activeDocumentsGroup;
    QActionGroup *recentDocumentsGroup;
    void addNewDocument();
    TextEditor *currentTextEditor();
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void updateTextStyleActions(const QFont &font);
    bool cursorAt(QTextCursor::MoveOperation op);
    void loadFonts();

private slots:
    void textAlignChanged(QAction *a);
    void paragraphStyleChanged(QAction *a);
    void currentDocumentChanged(int index);
    void clipboardDataChanged();
    void copyAvailableChanged(bool yes);
    void currentCharFormatChanged(const QTextCharFormat &format);
    void currentDocumentTitleChanged(const QString &newTitle);

    void on_actionNewDocument_triggered();
    void on_actionTextBold_triggered();
    void on_actionTextItalic_triggered();
    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void on_actionPaste_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionTextMath_triggered();
    void on_documentView_currentChanged(int index);

};

#endif // MAINWINDOW_H
