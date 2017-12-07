#ifndef DOCUMENTCOUNTER_H
#define DOCUMENTCOUNTER_H

#include <QTextBlock>
#include <QTextBlockGroup>
#include <QFont>
#include <QVector>
#include "document.h"
#include "blockproperties.h"

class Document;

class DocumentCounter : public QTextBlockGroup
{
private:
    int type;
    Document *doc;
    DocumentCounter *baseCounter;
    QString prefix;
    QString suffix;
    QFont font;

public:
    enum EnumerationType { Section = 1, Subsection = 2, Equation = 3, Figure = 4, Table = 5, User = 6 };
    DocumentCounter(Document *document, int t);
    inline void setBaseCounter(DocumentCounter *dc) { baseCounter = dc; }
    inline void setPrefix(QString p) { prefix = p; }
    inline void setSuffix(QString s) { suffix = s; }
    void setFont(QString &family, int pts, int wgh, bool itl);
    void updateBlockData();
    void addBlock(const QTextBlock &block);
    void removeBlock(const QTextBlock &block);
    QVector<int> getEnumerationAt(int position);
    int blockIndex(const QTextBlock &block);
};

#endif // DOCUMENTCOUNTER_H
