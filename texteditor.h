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

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QTextEdit>
#include <QAction>
#include <QActionGroup>
#include "document.h"

class TextEditor : public QTextEdit
{
    Q_OBJECT

public:
    TextEditor(Document *document, QWidget *parent = nullptr);
    ~TextEditor();
    inline Document *getDocument() { return doc; }
    void paragraphStyleChanged(int type);
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    bool cursorAtEndOfWord();
    QAction *createMenuAction(int index, QActionGroup *actionGroup);

signals:
    void focusRequested(int index);
    void canAddAuthor(bool yes);
    void canAddAffiliation(bool yes);
    void canAddEmail(bool yes);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    Document *doc;
    int lastBlockCount;
    QAction *menuAction;
    static int unnamedCount;
    QActionGroup *activeDocuments;

private slots:
    void menuActionTriggered(bool active);
    void blockCountChanged(int count);
    void cursorMoved();

};

#endif // TEXTEDITOR_H
