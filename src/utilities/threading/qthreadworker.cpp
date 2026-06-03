#include "qthreadworker.h"
#include "../logging/filelogger.h"

QThreadWorker::QThreadWorker(QObject *parent)
    : QObject(parent)
{
}

QThreadWorker::~QThreadWorker()
{
    stop();
}

void QThreadWorker::start()
{
    if (m_running)
        return;

    m_thread = new QThread(this);
    connect(m_thread, &QThread::started,  this, &QThreadWorker::onThreadStarted);
    connect(m_thread, &QThread::finished, this, &QThreadWorker::finished);
    moveToThread(m_thread);
    m_running = true;
    m_thread->start();
}

void QThreadWorker::stop()
{
    if (!m_running || !m_thread)
        return;
    m_thread->quit();
    m_thread->wait();
    m_running = false;
}

bool QThreadWorker::isRunning() const
{
    return m_running;
}

void QThreadWorker::onThreadStarted()
{
    emit workStarted();
    doWork();
    emit finished();
}

void QThreadWorker::logDebug(const QString &msg)
{
    FileLogger::instance().debug(msg);
}

void QThreadWorker::logError(const QString &msg)
{
    FileLogger::instance().error(msg);
}
