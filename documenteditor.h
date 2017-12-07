#ifndef DOCUMENTEDITOR_H
#define DOCUMENTEDITOR_H

#include <QTextEdit>
#include "document.h"

class DocumentEditor : public QTextEdit
{
private:
    Document *doc;

public:
    DocumentEditor(QWidget *parent = nullptr);
    inline Document *document() { return doc; }
    inline void setDocument(Document *document) { doc = document; }

protected:
    void paintEvent(QPaintEvent *e);
};

#endif // DOCUMENTEDITOR_H
