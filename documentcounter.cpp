#include <QStringList>
#include <QFontMetrics>
#include "documentcounter.h"
#include "glyphs.h"

DocumentCounter::DocumentCounter(Document *document, int t) : QTextBlockGroup(document)
{
    type = t;
    doc = document;
    baseCounter = nullptr;
    prefix = "";
    suffix = "";
    font = QFont(doc->style.textBodyFontFamily, doc->style.fontPointSize, QFont::Normal, false);
}

void DocumentCounter::setFont(QString &family, int pts, int wgh, bool itl)
{
    font = QFont(family, pts, wgh, itl);
}

void DocumentCounter::addBlock(const QTextBlock &block)
{
    QTextBlockGroup::blockInserted(block);
    updateBlockData();
}

void DocumentCounter::removeBlock(const QTextBlock &block)
{
    QTextBlockGroup::blockRemoved(block);
    updateBlockData();
}

void DocumentCounter::updateBlockData()
{
    QVector<int> en;
    BlockProperties *props, *oldProps;
    QStringList enumerationStringList;
    QTextBlockFormat blockFormat;
    qreal indent = 0.0;
    QTextCursor cursor(doc);
    QFontMetrics fontMetrics(font);
    for (QList<QTextBlock>::iterator it = blockList().begin(); it != blockList().end(); ++it)
    {
        oldProps = (BlockProperties*)it->userData();
        en.clear();
        if (baseCounter != nullptr)
            en = baseCounter->getEnumerationAt(it->position());
        en.push_back(blockIndex(*it));
        props = new BlockProperties();
        props->setEnumerationType(type);
        enumerationStringList.clear();
        foreach (int n, en)
            enumerationStringList.append(QString::number(n));
        props->setBlockHeader(prefix + enumerationStringList.join(".") + suffix);
        props->markDirty(oldProps == nullptr || oldProps->blockHeader() != props->blockHeader());
        it->setUserData(props);
        indent = fontMetrics.width(props->blockHeader() + Glyphs::emQuad());
        blockFormat = it->blockFormat();
        blockFormat.setTextIndent(indent);
        cursor.setPosition(it->position());
        cursor.mergeBlockFormat(blockFormat);
        if (oldProps != nullptr)
            delete oldProps;
    }
}

QVector<int> DocumentCounter::getEnumerationAt(int position)
{
    QVector<int> en;
    if (baseCounter != nullptr)
        en = baseCounter->getEnumerationAt(position);
    en.push_back(0);
    int blockPosition = -1;
    foreach (const QTextBlock &block, blockList())
    {
        int pos = block.position();
        if (pos <= position && pos > blockPosition)
        {
            blockPosition = pos;
            en.back() = 1 + blockIndex(block);
        }
    }
    return en;
}

int DocumentCounter::blockIndex(const QTextBlock &block)
{
    int n = 0;
    foreach (const QTextBlock &b, blockList())
    {
        if (b < block)
            ++n;
    }
    return n;
}
