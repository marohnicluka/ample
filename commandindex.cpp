#include "commandindex.h"
#include <QFile>
#include <QDebug>
#include <QXmlStreamAttributes>
#include <QRegularExpression>
#include <QRegExp>
#include <QXmlStreamWriter>

#define GIACHELP "giachelp.xml"

QString CommandIndex::lastName = "";
QString CommandIndex::lastElement = "";

CommandIndex::CommandIndex(QObject *parent) : QObject(parent)
{
    loadCommands();
}

bool CommandIndex::loadCommands()
{
    QFile file(QString(":/doc/") + GIACHELP);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        qInfo() << tr("Failed to open file \"") + GIACHELP + "\".";
        return false;
    }
    qInfo() << tr("Loading Giac command index from ") + GIACHELP + "...";
    QXmlStreamReader reader(&file);
    QStringList references;
    QMap<QString,int> repeatingReferences;
    int nEntries = 0, nWords = 0, nExamples = 0, nReferences = 0, nWiki = 0;
    if (reader.readNextStartElement())
    {
        if (reader.name() == "commandindex")
        {
            while (reader.readNextStartElement())
            {
                if (reader.name() != "entry")
                {
                    reader.raiseError(tr("Expected an entry, found \"") + reader.name() + "\".");
                    break;
                }
                if (!reader.attributes().hasAttribute("type"))
                {
                    reader.raiseError(tr("No entry type attribute found."));
                    break;
                }
                Command cmd;
                QString typeString = reader.attributes().value("type").toString();
                if (typeString == "command")
                    cmd.type = CmdTypeCommand;
                else if (typeString == "variable")
                    cmd.type = CmdTypeVariable;
                else if (typeString == "keyword")
                    cmd.type = CmdTypeKeyword;
                else if (typeString == "option")
                    cmd.type = CmdTypeOption;
                else if (typeString == "constant")
                    cmd.type = CmdTypeConstant;
                else if (typeString == "operator")
                    cmd.type = CmdTypeOperator;
                else
                {
                    reader.raiseError(tr("Unknown entry type \"") + typeString + "\".");
                    break;
                }
                while (reader.readNextStartElement())
                {
                    lastElement = reader.name().toString();
                    if (reader.name() == "name")
                    {
                        cmd.names = reader.readElementText().split(' ', QString::SkipEmptyParts);
                        nWords += cmd.names.length();
                        lastName = cmd.names.front();
                    }
                    else if (reader.name() == "category")
                        cmd.categories = reader.readElementText().split(',', QString::SkipEmptyParts);
                    else if (reader.name() == "parameters")
                    {
                        if (!parseParameters(cmd, reader) || reader.hasError())
                            break;
                    }
                    else if (reader.name() == "description")
                    {
                        Q_ASSERT(reader.attributes().hasAttribute("lang"));
                        Description description;
                        description.language = reader.attributes().value("lang").toString();
                        description.text = parseHTMLBlock(reader).replace(QRegExp(",(?!\\s)"), ", ");
                        cmd.descriptions.append(description);
                    }
                    else if (reader.name() == "related")
                        cmd.related.append(reader.readElementText());
                    else if (reader.name() == "reference")
                    {
                        Reference reference;
                        if (reader.attributes().hasAttribute("lang"))
                            reference.language = reader.attributes().value("lang").toString();
                        if (reader.attributes().hasAttribute("source"))
                        {
                            reference.source = reader.attributes().value("source").toString();
                            if (reference.source == "wiki")
                                nWiki++;
                        }
                        reference.title = parseHTMLBlock(reader);
                        cmd.references.append(reference);
                        nReferences++;
                        if (reference.language.length() == 0)
                        {
                            QString ref = QString("%1:%2").arg(reference.source).arg(reference.title);
                            if (references.contains(ref))
                                repeatingReferences[ref]++;
                            else
                                references.append(ref);
                        }
                    }
                    else if (reader.name() == "example")
                    {
                        cmd.examples.append(reader.readElementText());
                        nExamples++;
                    }
                    else
                        reader.raiseError(tr("Unknown entry element \"") + reader.name() + "\".");
                    if (reader.hasError())
                        break;
                }
                if (reader.hasError())
                    break;
                commands.append(cmd);
                nEntries++;
            }
        }
        else reader.raiseError(tr("Root element type mismatch."));
    }
    else reader.raiseError(tr("No root element found."));
    file.close();
    if (reader.hasError())
    {
        qInfo() << tr("Error:") + QString(" (%1, %2) ").arg(lastName).arg(lastElement) + reader.errorString();
        return false;
    }
    qInfo() << QString(tr("Successfully parsed %1 entries.")).arg(nEntries);
    qInfo() << QString(tr("Loaded %1 words, %2 examples and %3 references (%4 different, %5 from Wikipedia).").arg(
                           nWords).arg(nExamples).arg(nReferences).arg(references.length()).arg(nWiki));
    /*
    QMap<QString,int>::ConstIterator it = repeatingReferences.begin();
    for (; it != repeatingReferences.end(); ++it)
        qInfo() << QString("Reference to %1 is mentioned %2 times").arg(it.key()).arg(1 + it.value());
    */
    return true;
}

QString Command::description(const QString &lang) const
{
    Description desc;
    foreach (desc, descriptions)
    {
        if (desc.language == lang)
            return desc.text;
    }
    return "";
}

QString CommandIndex::parseHTMLBlock(QXmlStreamReader &reader)
{
    QString terminatingElement = reader.name().toString();
    QString html, name;
    QXmlStreamWriter writer(&html);
    do
    {
        reader.readNext();
        switch (reader.tokenType())
        {
        case QXmlStreamReader::StartElement:
            name = reader.name().toString();
            if (name == "tt")
            {
                writer.writeStartElement("font");
                writer.writeAttribute("face","freemono");
            }
            else
            {
                writer.writeStartElement(name);
                writer.writeAttributes(reader.attributes());
            }
            break;
        case QXmlStreamReader::EndElement:
            writer.writeEndElement();
            break;
        case QXmlStreamReader::Characters:
            writer.writeCharacters(reader.text().toString());
            break;
        case QXmlStreamReader::NoToken:
        case QXmlStreamReader::Invalid:
        case QXmlStreamReader::StartDocument:
        case QXmlStreamReader::EndDocument:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::DTD:
        case QXmlStreamReader::EntityReference:
        case QXmlStreamReader::ProcessingInstruction:
            break;
        }
    }
    while (!reader.atEnd() && reader.name() != terminatingElement);
    return html;
}

bool CommandIndex::parseParameters(Command &cmd, QXmlStreamReader &reader)
{
    InputSyntax syntax;
    if (reader.attributes().hasAttribute("ret")) {
        QStringList returnTypes = reader.attributes().value("ret").toString().split("||");
        QString returnType;
        QRegularExpression rx("^(\\w+)\\((\\w+)\\)$");
        foreach (returnType, returnTypes)
        {
            ReturnType ret;
            QRegularExpressionMatch match = rx.match(returnType);
            if (match.hasMatch())
            {
                ret.type = CommandParameter::parseParameterName(match.captured(1));
                ret.subtype = CommandParameter::parseParameterName(match.captured(2));
                if (ret.type == ParamTypeNone)
                    reader.raiseError(tr("Unrecognized parameter type \"") + match.captured(1) + "\".");
                if (ret.subtype == ParamTypeNone)
                    reader.raiseError(tr("Unrecognized parameter subtype \"") + match.captured(2) + "\".");
            }
            else
            {
                ret.type = CommandParameter::parseParameterName(returnType);
                if (ret.type == ParamTypeNone)
                    reader.raiseError(tr("Unrecognized parameter type \"") + returnType + "\".");
                ret.subtype = ParamTypeNone;
            }
            if (reader.hasError())
                return false;
            syntax.returnTypes.append(ret);
        }
    }
    while (reader.readNextStartElement())
    {
        CommandParameter parameter = parseParameter(reader);
        if (reader.hasError() || !parameter.isValid())
            return false;
        syntax.parameters.append(parameter);
    }
    cmd.syntaxes.append(syntax);
    return true;
}

CommandParameter CommandIndex::parseParameter(QXmlStreamReader &reader)
{
    CommandParameter parameter(reader.name().toString(), reader.attributes());
    if (parameter.isValid())
    {
        if (parameter.isComposite())
        {
            while (reader.readNextStartElement())
            {
                CommandParameter subParameter = parseParameter(reader);
                if (reader.hasError() || !subParameter.isValid())
                    break;
                parameter.children.append(subParameter);
            }
        }
        else parameter.text = reader.readElementText();
        if (reader.hasError() || !parameter.isValid())
            parameter.type = ParamTypeNone;
    }
    return parameter;
}

bool CommandParameter::isComposite()
{
    return type == ParamTypeInterval || type == ParamTypeEquation || type == ParamTypeChoice || explicite;
}

bool CommandParameter::isFixed()
{
    return type == ParamTypeEmpty || type == ParamTypeEllipsis || type == ParamTypeZero;
}

CommandParameter::CommandParameter(const QString &name, const QXmlStreamAttributes &attributes)
{
    type = parseParameterName(name);
    optional = false;
    optionalLhs = false;
    optionalRhs = false;
    explicite = false;
    dimension = 0;
    elementType = ParamTypeNone;
    alternateType = ParamTypeNone;
    if (attributes.hasAttribute("optional"))
        optional = (attributes.value("optional").toString() == "true");
    if (type == ParamTypeEquation)
    {
        if (attributes.hasAttribute("optionallhs"))
            optionalLhs = (attributes.value("optionallhs").toString() == "true");
        if (attributes.hasAttribute("optionalrhs"))
            optionalRhs = (attributes.value("optionalrhs").toString() == "true");
    }
    if (type == ParamTypeGeometricObject && attributes.hasAttribute("dim"))
        dimension = attributes.value("dim").toInt();
    if (type == ParamTypeInteger && attributes.hasAttribute("dom"))
    {
        QStringList elements = attributes.value("dom").toString().split(",", QString::SkipEmptyParts);
        QString element;
        foreach (element, elements)
            domain.append(element.toInt());
    }
    if (type == ParamTypeChoice && attributes.hasAttribute("name"))
        text = attributes.value("name").toString();
    if (type == ParamTypeInterval)
        elementType = ParamTypeReal;
    if (type == ParamTypeSequence || type == ParamTypeList || type == ParamTypeSet ||
            type == ParamTypeVector || type == ParamTypeMatrix)
    {
        if (attributes.hasAttribute("elem"))
            elementType = parseParameterName(attributes.value("elem").toString());
        if (attributes.hasAttribute("alt"))
            alternateType = parseParameterName(attributes.value("alt").toString());
        if (attributes.hasAttribute("explicit"))
            explicite = (attributes.value("explicit").toString() == "true");
    }
}

CommandParameterType CommandParameter::parseParameterName(const QString &name)
{
    if (name == "obj")
        return ParamTypeObject;
    if (name == "cmd")
        return ParamTypeCommand;
    if (name == "char")
        return ParamTypeCharacter;
    if (name == "file")
        return ParamTypeFile;
    if (name == "func")
        return ParamTypeFunction;
    if (name == "strng")
        return ParamTypeString;
    if (name == "bool")
        return ParamTypeBoolean;
    if (name == "table")
        return ParamTypeTable;
    if (name == "mtrx")
        return ParamTypeMatrix;
    if (name == "expr")
        return ParamTypeExpression;
    if (name == "var")
        return ParamTypeVariable;
    if (name == "interval")
        return ParamTypeInterval;
    if (name == "real")
        return ParamTypeReal;
    if (name == "intg")
        return ParamTypeInteger;
    if (name == "cplx")
        return ParamTypeComplex;
    if (name == "cycle")
        return ParamTypeCycle;
    if (name == "permut")
        return ParamTypePermutation;
    if (name == "unit")
        return ParamTypeUnit;
    if (name == "poly")
        return ParamTypePolynomial;
    if (name == "pnt")
        return ParamTypePoint;
    if (name == "line")
        return ParamTypeLine;
    if (name == "plane")
        return ParamTypePlane;
    if (name == "circle")
        return ParamTypeCircle;
    if (name == "segment")
        return ParamTypeSegment;
    if (name == "geo")
        return ParamTypeGeometricObject;
    if (name == "geo2")
        return ParamTypeGeometricObject2D;
    if (name == "geo3")
        return ParamTypeGeometricObject3D;
    if (name == "option")
        return ParamTypeOption;
    if (name == "equal")
        return ParamTypeEquation;
    if (name == "seq")
        return ParamTypeSequence;
    if (name == "lst")
        return ParamTypeList;
    if (name == "vect")
        return ParamTypeVector;
    if (name == "set")
        return ParamTypeSet;
    if (name == "quoted")
        return ParamTypeQuoted;
    if (name == "or")
        return ParamTypeChoice;
    if (name == "el")
        return ParamTypeElement;
    if (name == "etc")
        return ParamTypeEllipsis;
    if (name == "null")
        return ParamTypeEmpty;
    if (name == "zero")
        return ParamTypeZero;
    return ParamTypeNone;
}

QString CommandIndex::paramTypeToString(CommandParameterType paramType, int mode)
{
    switch (paramType)
    {
    case ParamTypeBoolean:
        return mode == 0 ? tr("boolean") : (mode == 1 ? tr("of booleans") : "bool");
    case ParamTypeCharacter:
        return mode == 0 ? tr("character") : (mode == 1 ? tr("of characters") : "char");
    case ParamTypeCircle:
        return mode == 0 ? tr("circle") : (mode == 1 ? tr("of circles") : "circle");
    case ParamTypeCommand:
        return mode == 0 ? tr("command") : (mode == 1 ? tr("of commands") : "cmd");
    case ParamTypeComplex:
        return mode == 0 ? tr("complex number") : (mode == 1 ? tr("of complex numbers") : "cplx");
    case ParamTypeCycle:
        return mode == 0 ? tr("cycle") : (mode == 1 ? tr("of cycles") : "cycle");
    case ParamTypeExpression:
        return mode == 0 ? tr("expression") : (mode == 1 ? tr("of expressions") : "expr");
    case ParamTypeFile:
        return mode == 0 ? tr("file") : (mode == 1 ? tr("of files") : "file");
    case ParamTypeFunction:
        return mode == 0 ? tr("function") : (mode == 1 ? tr("of functions") : "func");
    case ParamTypeGeometricObject:
        return mode == 0 ? tr("geometric object") : (mode == 1 ? tr("of geometric objects") : "geo");
    case ParamTypeGeometricObject2D:
        return mode == 0 ? tr("geometric object (2D)") : (mode == 1 ? tr("of 2D geometric objects") : "geo2");
    case ParamTypeGeometricObject3D:
        return mode == 0 ?  tr("geometric object (3D)") : (mode == 1 ? tr("of 3D geometric objects") : "geo3");
    case ParamTypeInteger:
        return mode == 0 ? tr("integer") : (mode == 1 ? tr("of integers") : "intg");
    case ParamTypeInterval:
        return mode == 0 ? tr("interval") : (mode == 1 ? tr("of intervals") : "interval");
    case ParamTypeLine:
        return mode == 0 ? tr("line") : (mode == 1 ? tr("of lines") : "line");
    case ParamTypeList:
        return mode == 0 ? tr("list") : (mode == 1 ? tr("of lists") : "lst");
    case ParamTypeMatrix:
        return mode == 0 ? tr("matrix") : (mode == 1 ? tr("of matrices") : "mtrx");
    case ParamTypeObject:
        return mode == 0 ? tr("object") : (mode == 1 ? tr("of objects") : "obj");
    case ParamTypeOption:
        return mode == 0 ? tr("option") : (mode == 1 ? tr("of options") : "option");
    case ParamTypePermutation:
        return mode == 0 ? tr("permutation") : (mode == 1 ? tr("of permutations") : "permut");
    case ParamTypePlane:
        return mode == 0 ? tr("plane") : (mode == 1 ? tr("of planes") : "plane");
    case ParamTypePoint:
        return mode == 0 ? tr("point") : (mode == 1 ? tr("of points") : "pnt");
    case ParamTypePolynomial:
        return mode == 0 ? tr("polynomial") : (mode == 1 ? tr("of polynomials") : "poly");
    case ParamTypeQuoted:
        return mode == 0 ? tr("quoted") : (mode == 1 ? tr("of quotations") : "quoted");
    case ParamTypeReal:
        return mode == 0 ? tr("real number") : (mode == 1 ? tr("of real numbers") : "real");
    case ParamTypeSegment:
        return mode == 0 ? tr("segment") : (mode == 1 ? tr("of segments") : "segment");
    case ParamTypeSequence:
        return mode == 0 ? tr("sequence") : (mode == 1 ? tr("of sequences") : "seq");
    case ParamTypeSet:
        return mode == 0 ? tr("set") : (mode == 1 ? tr("of sets") : "set");
    case ParamTypeString:
        return mode == 0 ? tr("string") : (mode == 1 ? tr("of strings") : "strng");
    case ParamTypeTable:
        return mode == 0 ? tr("table") : (mode == 1 ? tr("of tables") : "table");
    case ParamTypeUnit:
        return mode == 0 ? tr("unit") : (mode == 1 ? tr("of units") : "unit");
    case ParamTypeVariable:
        return mode == 0 ? tr("variable") : (mode == 1 ? tr("of variables") : "var");
    case ParamTypeVector:
        return mode == 0 ? tr("vector") : (mode == 1 ? tr("of vectors") : "vect");
    case ParamTypeElement:
        return mode == 2 ? "el" : "";
    case ParamTypeEllipsis:
        return mode == 0 ? "…" : "etc";
    case ParamTypeZero:
        return mode == 0 ? "0" : "zero";
    case ParamTypeEmpty:
        return mode == 0 ? "NULL" : "null";
    case ParamTypeEquation:
        return mode == 2 ? "equal" : "";
    case ParamTypeChoice:
        return mode == 2 ? "or" : "";
    default:
        return "";
    }
}

ParameterStyle CommandParameter::style(bool isElement)
{
    switch (isElement ? elementType : type)
    {
    case ParamTypeBoolean:
    case ParamTypeCharacter:
    case ParamTypeCircle:
    case ParamTypeComplex:
    case ParamTypeCycle:
    case ParamTypeExpression:
    case ParamTypeGeometricObject:
    case ParamTypeGeometricObject2D:
    case ParamTypeGeometricObject3D:
    case ParamTypeInteger:
    case ParamTypeFunction:
    case ParamTypeLine:
    case ParamTypePlane:
    case ParamTypeReal:
    case ParamTypeSegment:
    case ParamTypePermutation:
    case ParamTypePoint:
    case ParamTypePolynomial:
    case ParamTypeVariable:
    case ParamTypeObject:
    case ParamTypeUnit:
    case ParamTypeInterval:
        return ParamStyleItalic;
    case ParamTypeQuoted:
    case ParamTypeOption:
    case ParamTypeCommand:
    case ParamTypeFile:
        return ParamStyleMono;
    case ParamTypeMatrix:
    case ParamTypeTable:
    case ParamTypeList:
    case ParamTypeSequence:
    case ParamTypeString:
    case ParamTypeSet:
    case ParamTypeVector:
        return ParamStyleBold;
    case ParamTypeElement:
    case ParamTypeEllipsis:
    case ParamTypeZero:
    case ParamTypeEmpty:
    case ParamTypeEquation:
    case ParamTypeChoice:
    default:
        return ParamStyleNormal;
    }
}

QString CommandParameter::formatText(const QString &text, ParameterStyle parameterStyle)
{
    if (text.length() == 0)
        return text;
    QString tag, styledText;
    switch (parameterStyle)
    {
    case ParamStyleBold:
        tag = "b";
        break;
    case ParamStyleItalic:
        tag = "i";
        break;
    case ParamStyleMono:
        return toMono(text);
    case ParamStyleNormal:
        break;
    }
    if (text.length() == 1 && !text.at(0).isDigit())
        return QString("<%1>%2</%1>").arg(tag).arg(text);
    QRegularExpressionMatchIterator matchIterator =
            QRegularExpression("\\b([a-zA-Z]+)(_[a-zA-Z0-9]+|[0-9]\\b)?").globalMatch(text);
    int position = 0;
    while (matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();
        styledText += text.mid(position, match.capturedStart() - position);
        QString idnt = match.captured(1);
        if (idnt.length() == 1)
            styledText += QString("<%1>%2</%1>").arg(tag).arg(idnt);
        else if (idnt.length() == 2 && idnt.at(0) == idnt.at(1))
            styledText += QString("<b>%1</b>").arg(idnt.left(1));
        else
            styledText += toMono(idnt);
        if (match.capturedStart(2) >= 0)
        {
            QString subscript = match.captured(2);
            if (subscript.startsWith("_"))
                subscript = subscript.mid(1);
            if (subscript.length() == 1 && !subscript.at(0).isDigit())
                subscript = QString("<i>%1</i>").arg(subscript);
            styledText += QString("<sub>%1</sub>").arg(subscript);
        }
        position = match.capturedEnd();
    }
    styledText += text.mid(position);
    return styledText;
}

QString CommandParameter::format(ParameterStyle elementStyle)
{
    QString formattedText;
    ParameterStyle parameterStyle = type == ParamTypeElement ? elementStyle : style();
    formattedText = CommandIndex::paramTypeToString(type, 0);
    if (isFixed())
        return formattedText;
    if (alternateType != ParamTypeNone)
        formattedText += QString(" %1 %2").arg(CommandIndex::orSeparator()).arg(
                    CommandIndex::paramTypeToString(alternateType, 0));
    if (elementType != ParamTypeNone && type != ParamTypeInterval)
        formattedText += QString(" %1").arg(CommandIndex::paramTypeToString(elementType, 1));
    if (!children.empty())
    {
        ParameterStyle childStyle = elementType != ParamTypeNone ? style(true) : elementStyle;
        QStringList formattedChildren;
        CommandParameter child;
        foreach (child, children)
            formattedChildren.append(child.format(childStyle).trimmed());
        QString str;
        switch (type)
        {
        case ParamTypeInterval:
            str = formattedChildren.front() + " ‥ " + formattedChildren.back();
            break;
        case ParamTypeEquation:
            if (optionalLhs)
                str = QString("[%1 =] %2").arg(formattedChildren.front()).arg(formattedChildren.back());
            else if (optionalRhs)
                str = QString("%1 [= %2]").arg(formattedChildren.front()).arg(formattedChildren.back());
            else
                str = formattedChildren.front() + " = " + formattedChildren.back();
            break;
        case ParamTypeChoice:
            str = formattedChildren.join(QString(" %1 ").arg(CommandIndex::orSeparator()));
            break;
        case ParamTypeSequence:
            str = QString("(%1)").arg(formattedChildren.join(", "));
            break;
        case ParamTypeSet:
            str = QString("{%1}").arg(formattedChildren.join(", "));
            break;
        case ParamTypeList:
        case ParamTypeVector:
            str = QString("[%1]").arg(formattedChildren.join(", "));
            break;
        default:
            break;
        }
        formattedText += QString(" %1").arg(str);
    }
    if (text.length() > 0)
        formattedText += QString(" %1").arg(formatText(text.replace("-", "−"), parameterStyle));
    if (!domain.empty())
    {
        formattedText += domain.length() > 1 ? QString(" %1 {").arg(CommandIndex::inSeparator()) : " ";
        QStringList domainElements;
        int elem;
        foreach (elem, domain)
            domainElements.append(QString::number(elem).replace("-", "−"));
        formattedText += domainElements.join(", ");
        if (domain.length() > 1)
            formattedText += "}";
    }
    if (optional)
        formattedText += QString(" (%1)").arg(CommandIndex::optionalNotice());
    return formattedText;
}
