#ifndef TEXTFRAMEGROUP_H
#define TEXTFRAMEGROUP_H

#include <QObject>
#include <QVector>
#include <QTextFrame>
#include <QTextCursor>
#include <QTextDocument>

class TextFrameGroup : public QObject
{
    Q_OBJECT

private:
    QVector<QTextFrame*> frames;
    bool onlyAllowedInRootFrame;

public:
    explicit TextFrameGroup(QObject *parent = nullptr);
    inline void setOnlyAllowedInRootFrame(bool allowed) { onlyAllowedInRootFrame = allowed; }
    void addFrame(QTextCursor &cursor);
    QTextFrame *frameForCursor(const QTextCursor &cursor);

signals:

public slots:
};

#endif // TEXTFRAMEGROUP_H
