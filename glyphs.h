#ifndef GLYPHS_H
#define GLYPHS_H

#include <QObject>

class Glyphs : public QObject
{
    Q_OBJECT
public:
    explicit Glyphs(QObject *parent = nullptr);

    static inline QChar emQuad() { return QChar(0x2001); }
};

#endif // GLYPHS_H
