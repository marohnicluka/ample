#ifndef SESSION_H
#define SESSION_H
//#define _GLIBCXX_USE_CXX11_ABI 0

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QPicture>
#include <QStringList>
#include <QMap>
#include <QStack>
#include <QVector>
#include <QFont>
#include <QPainter>
#include <QFlags>
#include <QRegularExpression>
#include <QStringList>
#include <qmath.h>
#include <streambuf>
#include <giac/config.h>
#include <giac/giac.h>
#include "mathglyphs.h"

using namespace giac;

class MonitorThread : public QThread
{
    Q_OBJECT
public:
    MonitorThread(context *ct);

protected:
    void run();

private:
    context* contextptr;
};

class StopThread : public QThread
{
    Q_OBJECT
public:
    StopThread(giac::context *ct);

protected:
    void run();

private:
    bool go;
    context* contextptr;

public slots:
    void setContinueTrue();

signals:
    void startDirtyInterrupt();
};

class Session;

class MessageBuffer : public std::streambuf
{
    void putBuffer(void);
    void putChar(int);
    Session *session;

protected:
    int overflow(int);
    int sync();

public:
    MessageBuffer(Session *s, int bsize = 0);
};

class MessageStream : public std::ostream
{
public:
    MessageStream(Session *s, int bsize = 0);
};

class Session : public QObject
{
    Q_OBJECT
    context *ct;
    MonitorThread *monitor;
    StopThread *stopThread;
    MessageStream *messageStream;
    QString printCache;
    QStringList messages;
    static gen answer;
    static void callback(const gen &g, void *newcontextptr);

public:
    explicit Session(QObject *parent = nullptr);
    ~Session();
    const context *getContext() const { return ct; }
    gen getAnswer() const { return answer; }
    QStringList &getGiacMessages();
    void appendPrintCache(const QChar &c);
    void clearGiacMessages() { messages.clear(); }
    bool evaluate(const gen &g);
    void killThread();
    bool isRunning() const { return monitor->isRunning(); }

signals:
    void processingStarted();
    void processingFinished(const gen &result, const QStringList &messages);
    void killingThread();

public slots:
    void resultReady();

};

#endif // SESSION_H
