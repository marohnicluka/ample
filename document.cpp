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

#include "document.h"

using namespace giac;

Document::Document(GIAC_CONTEXT, QObject *parent) :
    QTextDocument(parent)
{
    gcontext = contextptr;
    ghighlighter = new GiacHighlighter(this);
    style.textBodyFontFamily = "FreeSerif";
    style.headingsFontFamily = "FreeSans";
    style.casInputFontFamily = "FreeMono";
    style.fontPointSize = 12.0;
    style.groundRatio = 1.324718;
    worksheetMode = false;
}

bool Document::isHeading(const QTextBlock &block) {
    int type = block.blockFormat().intProperty(ParagraphStyleId);
    return type == Document::Title || type == Document::Section || type == Document::Subsection;
}
