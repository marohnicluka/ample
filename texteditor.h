#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QTextEdit>
#include "document.h"

class TextEditor : public QTextEdit
{
private:
    Document *doc;
    int lastBlockCount;

public:
    TextEditor(Document *document, QWidget *parent = nullptr);
    void blockCountChanged(int count);
    void paragraphStyleChanged(int type);
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    bool cursorAt(QTextCursor::MoveOperation op);

protected:
    void paintEvent(QPaintEvent *e) override;

};

#endif // TEXTEDITOR_H
