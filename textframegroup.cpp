#include "textframegroup.h"

TextFrameGroup::TextFrameGroup(QObject *parent) : QObject(parent)
{

}

QTextFrame* TextFrameGroup::frameForCursor(const QTextCursor &cursor)
{
    QTextFrame *root = cursor.document()->rootFrame(), *frame = cursor.currentFrame();
    if (onlyAllowedInRootFrame)
    {
        while (frame)
        {
            if (frame->parentFrame() == root)
                break;
            frame = frame->parentFrame();
        }
    }
    if (frame)
    {
        QVector<QTextFrame*>::const_iterator it;
        for (it = frames.begin(); it != frames.end(); ++it)
        {
            if (*it == frame)
                return frame;
        }
    }
    return 0;
}
