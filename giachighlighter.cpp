/*
 * This file is part of Ample.
 *
 * Ample is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ample is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ample.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "giachighlighter.h"

QStringList GiacHighlighter::readWords(QXmlStreamReader *reader, bool boundaries)
{
    QStringList words;
    QString word;
    reader->readNext();
    while (reader->tokenType() != QXmlStreamReader::EndElement || reader->name() != "context")
    {
        if (reader->name() == "keyword")
        {
            word = reader->readElementText();
            if (boundaries)
            {
                if (word.contains("|"))
                {
                    word.append(")");
                    word.prepend("(");
                }
                word.append("\\b");
                word.prepend("\\b");
            }
            words.append(word);
        }
        reader->readNext();
    }
    return words;
}

void GiacHighlighter::createRulesFrom(QStringList &words, QTextCharFormat &fmt)
{
    HighlightingRule rule;
    foreach (const QString word, words)
    {
        rule.pattern = QRegExp(word);
        rule.format = fmt;
        rules.append(rule);
    }
}

void GiacHighlighter::setFormatProperties(QTextCharFormat *format, const QBrush &color, bool bold, bool italic)
{
    if (color != Qt::black)
        format->setForeground(color);
    format->setFontWeight(bold ? QFont::Bold : QFont::Normal);
    format->setFontItalic(italic);
}

GiacHighlighter::GiacHighlighter(Worksheet *parent) : QSyntaxHighlighter(parent)
{
    doc = parent;
    QStringList keywords,variables,options,commands,constants,units;
    QFile file(":/giac-keywords.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning("Loading error: couldn't open giac-keywords.xml");
        return;
    }
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(&file);
    while (!xmlReader.atEnd() && !xmlReader.hasError())
    {
        QXmlStreamReader::TokenType token=xmlReader.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement)
        {
            if (xmlReader.name() == "language")
                continue;
            if (xmlReader.name() == "context")
            {
                QXmlStreamAttributes attributes=xmlReader.attributes();
                if (attributes.hasAttribute("id")) {
                    QString type = attributes.value("id").toString();
                    if (type == "keyword")
                        keywords = readWords(&xmlReader);
                    else if (type == "variable")
                        variables = readWords(&xmlReader);
                    else if (type == "option")
                        options = readWords(&xmlReader);
                    else if (type == "command")
                        commands = readWords(&xmlReader);
                    else if (type == "constant")
                        constants = readWords(&xmlReader);
                    else if (type == "unit")
                        units = readWords(&xmlReader, false);
                }
            }
        }
    }
    if (xmlReader.hasError())
    {
        qWarning() << "Parsing error: " << xmlReader.errorString() << "\n";
        return;
    }
    xmlReader.clear();
    setFormatProperties(&defaultFormat, Qt::black, false, false);
    setFormatProperties(&keywordFormat, Qt::darkBlue, true, false);
    setFormatProperties(&variableFormat, Qt::darkRed, true, false);
    setFormatProperties(&optionFormat, Qt::darkRed, true, false);
    setFormatProperties(&commandFormat, Qt::darkRed, true, false);
    setFormatProperties(&constantFormat, Qt::darkCyan, true, false);
    setFormatProperties(&unitFormat, Qt::darkMagenta, true, false);
    setFormatProperties(&stringFormat,Qt::darkGreen,false,false);
    setFormatProperties(&commentFormat, Qt::darkGray, false, true);
    setFormatProperties(&operatorFormat, Qt::darkBlue, true, false);
    setFormatProperties(&numberFormat, Qt::black, false, false);
    createRulesFrom(keywords,keywordFormat);
    createRulesFrom(variables,variableFormat);
    createRulesFrom(options,optionFormat);
    createRulesFrom(commands,commandFormat);
    createRulesFrom(constants,constantFormat);
    HighlightingRule rule;
    rule.pattern = QRegExp("_[YZEPTGMKkHhDdcmµnpfazy]?(" + units.join("|") + ")\\b");
    rule.format = unitFormat;
    rules.append(rule);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = commentFormat;
    rules.append(rule);
    rule.pattern = QRegExp("\".*\"");
    rule.format = stringFormat;
    rules.append(rule);
    rule.pattern = QRegExp("\\d*\\.?\\d+(e[\\-\\+]?\\d+)?");
    rule.format = numberFormat;
    rules.append(rule);
    rule.pattern = QRegExp("\\b(and|et|o[ru]|div|minus|union|intersect|mod)\\b");
    rule.format = operatorFormat;
    rules.append(rule);
}

void GiacHighlighter::highlightBlock(const QString &text)
{
    QTextCursor cursor(doc);
    cursor.setPosition(currentBlock().position());
    QTextFrame *frame = cursor.currentFrame();
    if (!doc->isCasInputFrame(frame))
        return;
    UnionOfRanges unhighlightedRanges(0, text.length());
    int pos, len;
    foreach (const HighlightingRule &rule, rules)
    {
        pos = 0;
        while ((pos = rule.pattern.indexIn(text, pos)) != -1)
        {
            len = rule.pattern.matchedLength();
            unhighlightedRanges.cutOut(pos,len);
            setFormat(pos, len, rule.format);
            pos += len;
        }
    }
    int n = 0;
    while (unhighlightedRanges.nthRange(n++, pos, len))
        setFormat(pos, len, defaultFormat);
}

void UnionOfRanges::cutOut(int start, int length)
{
    int end = start + length;
    for (int i = ranges.size() - 1; i >= 0; --i)
    {
        int a = ranges.at(i).first, b = ranges.at(i).second;
        if (start > b || end < a)
            continue;
        if (start <= a && end >= b)
            ranges.remove(i);
        else if (start <= a && end < b)
            ranges[i].first = end;
        else if (start > a && end >= b)
            ranges[i].second = start;
        else
        {
            ranges.push_back(QPair<int, int>(end, ranges.at(i).second));
            ranges[i].second = start;
        }
    }
}

bool UnionOfRanges::nthRange(int n, int &start, int &length)
{
    if (n >= nRanges())
        return false;
    start = ranges.at(n).first;
    length = ranges.at(n).second - start;
    return true;
}
