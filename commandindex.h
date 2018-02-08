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

enum ParameterStyle
{
    ParamStyleNormal,
    ParamStyleItalic,
    ParamStyleBold,
    ParamStyleMono
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

    CommandParameter() { }
    CommandParameter(const QString &name, const QXmlStreamAttributes &attributes);
    QString format(ParameterStyle elementStyle = ParamStyleNormal);
    bool isComposite();
    bool isFixed();
    bool isValid() { return type != ParamTypeNone; }
    static CommandParameterType parseParameterName(const QString &name);
    static QString toMono(const QString &text) { return QString("<font face=\"freemono\">%1</font>").arg(text); }
    static QString formatText(const QString &text, ParameterStyle parameterStyle);
    ParameterStyle style(bool isElement = false);
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

struct Reference
{
    QString title;
    QString source;
    QString language;
};

struct Description
{
    QString text;
    QString language;
};

struct Command
{
    CommandType type;
    QStringList names;
    QStringList categories;
    QList<InputSyntax> syntaxes;
    QList<Description> descriptions;
    QStringList related;
    QStringList examples;
    QList<Reference> references;

    QString description(const QString &lang) const;
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
    static QString paramTypeToString(CommandParameterType paramType, int mode);
    static QString optionalNotice() { return tr("optional"); }
    static QString orSeparator() { return tr("or"); }
    static QString inSeparator() { return tr("in"); }

signals:

public slots:
};

#endif // COMMANDINDEX_H
