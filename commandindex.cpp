#include "commandindex.h"
#include <QFile>
#include <QDebug>
#include <QXmlStreamAttributes>
#include <QRegularExpression>
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
    int nEntries = 0, nWords = 0, nExamples = 0;
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
                QString lang;
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
                        lang = reader.attributes().value("lang").toString();
                        cmd.descriptions[lang] = parseHTMLBlock(reader);
                    }
                    else if (reader.name() == "related")
                        cmd.related.append(reader.readElementText());
                    else if (reader.name() == "reference")
                    {
                        if (reader.attributes().hasAttribute("lang"))
                            lang = reader.attributes().value("lang").toString();
                        else
                            lang = "all";
                        cmd.references[lang] += QString("<p>%1</p>").arg(parseHTMLBlock(reader));
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
    qInfo() << QString(tr("Loaded %1 words and %2 examples.").arg(nWords).arg(nExamples));
    return true;
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

CommandParameter::CommandParameter(const QString &name, const QXmlStreamAttributes &attributes)
{
    type = parseParameterName(name);
    optional = false;
    optionalLhs = false;
    optionalRhs = false;
    explicite = false;
    dimension = 0;
    elementType = ParamTypeNone;
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
    if (type == ParamTypeSequence || type == ParamTypeList || type == ParamTypeSet ||
            type == ParamTypeVector || type == ParamTypeMatrix || type == ParamTypeQuoted)
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

QString CommandParameter::format()
{

}
