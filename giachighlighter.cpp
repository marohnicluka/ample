/*
 * This file is part of Giac Qt.
 *
 * Giac Qt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Giac Qt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Giac Qt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "giachighlighter.h"

QStringList GiacHighlighter::readWords(QXmlStreamReader *reader, bool boundaries) {
    QStringList words;
    QString word;
    reader->readNext();
    while (reader->tokenType() != QXmlStreamReader::EndElement || reader->name() != "context") {
        if (reader->name() == "keyword") {
            word = reader->readElementText();
            if (boundaries) {
                if (word.contains("|")) {
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

void GiacHighlighter::createRulesFrom(QStringList &words, QTextCharFormat &fmt) {
    HighlightingRule rule;
    foreach (const QString word, words) {
        rule.pattern = QRegExp(word);
        rule.format = fmt;
        rules.append(rule);
    }
}

GiacHighlighter::GiacHighlighter(Document *parent) : QSyntaxHighlighter(parent) {
    doc = parent;
    QStringList keywords,variables,options,commands,constants,units;
    QFile file(":/giac-keywords.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Loading error: couldn't open giac-keywords.xml");
        return;
    }
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(&file);
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {
        QXmlStreamReader::TokenType token=xmlReader.readNext();
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement) {
            if (xmlReader.name() == "language")
                continue;
            if (xmlReader.name() == "context") {
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
    if (xmlReader.hasError()) {
        qWarning() << "Parsing error: " << xmlReader.errorString() << "\n";
        return;
    }
    xmlReader.clear();
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Weight::Bold);
    variableFormat.setForeground(Qt::darkRed);
    variableFormat.setFontWeight(QFont::Weight::Bold);
    optionFormat.setForeground(Qt::darkRed);
    optionFormat.setFontWeight(QFont::Weight::Bold);
    commandFormat.setForeground(Qt::darkRed);
    commandFormat.setFontWeight(QFont::Weight::Bold);
    constantFormat.setForeground(Qt::darkCyan);
    constantFormat.setFontWeight(QFont::Weight::Bold);
    unitFormat.setForeground(Qt::darkMagenta);
    unitFormat.setFontWeight(QFont::Weight::Bold);
    //numberFormat.setForeground(Qt::darkCyan);
    stringFormat.setForeground(Qt::darkGreen);
    commentFormat.setForeground(Qt::darkGray);
    commentFormat.setFontItalic(true);
    operatorFormat.setForeground(Qt::darkBlue);
    operatorFormat.setFontWeight(QFont::Weight::Bold);
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

void GiacHighlighter::highlightBlock(const QString &text) {
    int pos, len;
    foreach (const HighlightingRule &rule, rules) {
        pos = 0;
        while ((pos = rule.pattern.indexIn(text,pos)) != -1) {
            len = rule.pattern.matchedLength();
            if (format(pos).fontFamily() == doc->style.casInputFontFamily)
                setFormat(pos, len, rule.format);
            pos += len;
        }
    }
}
