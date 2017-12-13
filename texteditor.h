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
#include "worksheet.h"

class TextEditor : public QTextEdit
{
    Q_OBJECT

public:
    TextEditor(Worksheet *worksheet, QWidget *parent = nullptr);
    ~TextEditor();
    inline Worksheet *worksheet() { return m_worksheet; }
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    bool cursorAtEndOfWord();
    QAction *createMenuAction(int index, QActionGroup *actionGroup);

signals:
    void focusRequested(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    Worksheet *m_worksheet;
    QAction *menuAction;
    QActionGroup *activeDocuments;
    static int unnamedCount;

private slots:
    void menuActionTriggered(bool active);
    void cursorMoved();

};

#endif // TEXTEDITOR_H
