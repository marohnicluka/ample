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
