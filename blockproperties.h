#ifndef BLOCKPROPERTIES_H
#define BLOCKPROPERTIES_H

#include <QTextBlockUserData>

class BlockProperties : public QTextBlockUserData
{
private:
    int type;
    QString header;
    bool dirty;

public:
    BlockProperties();
    inline int enumerationType() { return type; }
    inline const QString blockHeader() { return header; }
    inline void setEnumerationType(int t) { type = t; }
    inline void setBlockHeader(const QString &h) { header = h; }
    inline bool needsRedrawing() { return dirty; }
    inline void markDirty(bool yes) { dirty = yes; }
};

#endif // BLOCKPROPERTIES_H
