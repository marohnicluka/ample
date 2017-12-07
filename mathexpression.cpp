#include "mathexpression.h"

MathExpression::MathExpression(GIAC_CONTEXT, QObject *parent) : QObject(parent)
{
    gcontext = contextptr;
}
