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

#ifndef GIACHIGHLIGHTER_H
#define GIACHIGHLIGHTER_H

#include "document.h"
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QRegExp>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>

class Document;

class GiacHighlighter : public QSyntaxHighlighter {
private:
    Document *doc;
    struct HighlightingRule {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> rules;
    QTextCharFormat keywordFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat variableFormat;
    QTextCharFormat optionFormat;
    QTextCharFormat commandFormat;
    QTextCharFormat unitFormat;
    QTextCharFormat constantFormat;
    QTextCharFormat operatorFormat;
    QStringList readWords(QXmlStreamReader *reader, bool boundaries = true);
    void createRulesFrom(QStringList &words, QTextCharFormat &fmt);
public:
    GiacHighlighter(Document *parent);
    inline Document *document() { return doc; }
protected:
    void highlightBlock(const QString &text) override;
};

#endif // GIACHIGHLIGHTER_H
