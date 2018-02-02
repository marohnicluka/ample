#ifndef COMMANDINDEX_H
#define COMMANDINDEX_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QXmlStreamReader>

enum CommandType
{
    CmdTypeCommand,
    CmdTypeVariable,
    CmdTypeOption,
    CmdTypeConstant,
    CmdTypeKeyword,
    CmdTypeOperator
};

enum CommandParameterType
{
    ParamTypeNone,
    ParamTypeObject,
    ParamTypeCommand,
    ParamTypeCharacter,
    ParamTypeFile,
    ParamTypeFunction,
    ParamTypeString,
    ParamTypeBoolean,
    ParamTypeTable,
    ParamTypeMatrix,
    ParamTypeExpression,
    ParamTypeVariable,
    ParamTypeInterval,
    ParamTypeReal,
    ParamTypeInteger,
    ParamTypeComplex,
    ParamTypeCycle,
    ParamTypePermutation,
    ParamTypeUnit,
    ParamTypePolynomial,
    ParamTypePoint,
    ParamTypeLine,
    ParamTypePlane,
    ParamTypeCircle,
    ParamTypeSegment,
    ParamTypeGeometricObject,
    ParamTypeGeometricObject2D,
    ParamTypeGeometricObject3D,
    ParamTypeOption,
    ParamTypeEquation,
    ParamTypeSequence,
    ParamTypeList,
    ParamTypeVector,
    ParamTypeSet,
    ParamTypeQuoted,
    ParamTypeChoice,
    ParamTypeElement,
    ParamTypeEllipsis,
    ParamTypeEmpty,
    ParamTypeZero
};

struct CommandParameter
{
    CommandParameterType type;
    bool optional;
    bool explicite;
    bool optionalLhs;
    bool optionalRhs;
    QList<int> domain;
    int dimension;
    CommandParameterType elementType;
    CommandParameterType alternateType;
    QString text;
    QList<CommandParameter> children;

    CommandParameter(const QString &name, const QXmlStreamAttributes &attributes);
    QString format();
    bool isComposite();
    bool isValid() { return type != ParamTypeNone; }
    static CommandParameterType parseParameterName(const QString &name);
};

struct ReturnType
{
    CommandParameterType type;
    CommandParameterType subtype;
};

struct InputSyntax
{
    QList<CommandParameter> parameters;
    QList<ReturnType> returnTypes;
};

struct Command
{
    CommandType type;
    QStringList names;
    QStringList categories;
    QList<InputSyntax> syntaxes;
    QMap<QString,QString> descriptions;
    QStringList related;
    QStringList examples;
    QMap<QString,QString> references;
};

class CommandIndex : public QObject
{
    Q_OBJECT

    QList<Command> commands;
    static QString lastName;
    static QString lastElement;

    bool loadCommands();
    QString parseHTMLBlock(QXmlStreamReader &reader);

public:
    explicit CommandIndex(QObject *parent = 0);
    int commandCount() { return commands.length(); }
    const Command &commandAt(int index) { Q_ASSERT(index < commandCount()); return commands.at(index); }

    static bool parseParameters(Command &cmd, QXmlStreamReader &reader);
    static CommandParameter parseParameter(QXmlStreamReader &reader);

signals:

public slots:
};

#endif // COMMANDINDEX_H
