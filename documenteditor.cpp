#include "documenteditor.h"

DocumentEditor::DocumentEditor(QWidget *parent) : QTextEdit(parent)
{

}

void DocumentEditor::paintEvent(QPaintEvent *e)
{
    QTextEdit::paintEvent(e);
}
