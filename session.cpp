#include "session.h"

using namespace giac;

MonitorThread::MonitorThread(context *ct) : contextptr(ct) { }

void MonitorThread::run()
{
    while (true)
    {
        if (check_thread(contextptr) == 1)
            QThread::msleep(50);
        else break;
    }
}

StopThread::StopThread(giac::context *ct) : contextptr(ct) { }

void StopThread::run(){
    ctrl_c = true;
    sleep(2);
    if (check_thread(contextptr) == 1)
    {
        qInfo() << "Dirty try to interrupt thread!!!";
        go=false;
        emit(startDirtyInterrupt());
        while (!go) msleep(20);
        kill_thread(true, contextptr);
    }
    else qInfo() << "Clean interruption";
}

void StopThread::setContinueTrue()
{
    go = true;
}

MessageBuffer::MessageBuffer(Session *s, int bsize) : std::streambuf()
{
    session = s;
    if (bsize)
    {
        char *ptr = new char[bsize];
        setp(ptr, ptr + bsize);
    }
    else setp(0, 0);
    setg(0, 0, 0);
}

int MessageBuffer::overflow(int c)
{
    putBuffer();
    if (c != EOF)
    {
        if (pbase() == epptr())
            putChar(c);
        else
            sputc(c);
    }
    return 0;
}


int MessageBuffer::sync()
{
    putBuffer();
    return 0;
}

void MessageBuffer::putChar(int chr)
{
    QChar c(chr);
    session->appendPrintCache(c);
}

void MessageBuffer::putBuffer()
{
    if (pbase() != pptr())
    {
        int len = pptr() - pbase();
        char *buffer = new char[len + 1];
        strncpy(buffer, pbase(), len);
        buffer[len] = 0;
        setp(pbase(), epptr());
        delete [] buffer;
    }
}

MessageStream::MessageStream(Session *s, int bsize) : std::ostream(new MessageBuffer(s, bsize)) { }

gen Session::answer(undef);

Session::Session(QObject *parent) : QObject(parent)
{
    ct = new context;
    monitor = new MonitorThread(ct);
    stopThread = new StopThread(ct);
    messageStream = new MessageStream(this);
    signal(SIGINT, ctrl_c_signal_handler);
    logptr(messageStream, ct);
}

Session::~Session()
{
    delete ct;
    delete monitor;
    delete stopThread;
    delete messageStream;
}

void Session::appendPrintCache(const QChar &c)
{
    if (c == '"')
        printCache.append("&quot;");
    else if (c == '&')
        printCache.append("&amp;");
    else if (c == '<')
        printCache.append("&lt;");
    else if (c == '>')
        printCache.append("&gt;");
    else if (c == '\n')
    {
        messages.append(printCache);
        printCache = "";
    }
    else printCache.append(c);
}

QStringList& Session::getGiacMessages()
{
    if (!printCache.isEmpty())
        messages.append(printCache);
    return messages;
}

void Session::callback(const gen &g,void *newcontextptr)
{
    Q_UNUSED(newcontextptr)
    answer = g;
}

bool Session::evaluate(const gen &g)
{
    if (stopThread->isRunning())
        stopThread->wait(2000);
    printCache = "";
    if (make_thread(g,eval_level(ct), callback, (void*)ct, ct))
    {
        disconnect(monitor,SIGNAL(finished()),this,SLOT(resultReady()));
        monitor->start();
        connect(monitor,SIGNAL(finished()),this,SLOT(resultReady()));
        processingStarted();
    }
    else return false;
    history_in(ct).push_back(g);
    return true;
}

void Session::resultReady()
{
    history_out(ct).push_back(answer);
    processingFinished(answer, getGiacMessages());
}

void Session::killThread()
{
    if (!stopThread->isRunning()) {
        emit(killingThread());
        stopThread->start();
    }
}
