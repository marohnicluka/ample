#ifndef MATHEXPRESSION_H
#define MATHEXPRESSION_H

#include <QObject>
#include <giac/giac.h>

using namespace giac;

class MathExpression : public QObject
{
    Q_OBJECT

    const context *gcontext;
    QString expr;

public:
    explicit MathExpression(GIAC_CONTEXT, QObject *parent = nullptr);
    inline void setExpression(const gen &e) { expr = QString(e.print().c_str()); }
    inline const QString expression() { return expr; }
};

#endif // MATHEXPRESSION_H
