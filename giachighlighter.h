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

#ifndef GIACHIGHLIGHTER_H
#define GIACHIGHLIGHTER_H

#include "worksheet.h"
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QRegExp>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QVector>

class Worksheet;

class GiacHighlighter : public QSyntaxHighlighter
{
private:
    Worksheet *doc;
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> rules;
    QTextCharFormat defaultFormat;
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
    static void setFormatProperties(QTextCharFormat *format, const QBrush &color, bool bold, bool italic);

public:
    GiacHighlighter(Worksheet *parent);
    inline Worksheet *document() { return doc; }

protected:
    void highlightBlock(const QString &text) override;
};

class UnionOfRanges
{
private:
    QVector< QPair<int, int> > ranges;

public:
    UnionOfRanges(int start, int length) { ranges.push_back(QPair<int, int>(start, start + length)); }
    int nRanges() { return ranges.size(); }
    void cutOut(int start, int length);
    bool nthRange(int n, int &start, int &length);
};

#endif // GIACHIGHLIGHTER_H
