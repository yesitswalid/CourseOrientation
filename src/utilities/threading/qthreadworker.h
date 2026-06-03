#ifndef QTHREADWORKER_H
#define QTHREADWORKER_H

#include <QObject>
#include <QThread>
#include <QString>

class QThreadWorker : public QObject {
    Q_OBJECT
public:
    explicit QThreadWorker(QObject *parent = nullptr);
    ~QThreadWorker() override;

    void start();
    void stop();
    bool isRunning() const;

protected:
    virtual void doWork() = 0;
    void logDebug(const QString &msg);
    void logError(const QString &msg);

signals:
    void finished();
    void errorOccurred(const QString &error);
    void workStarted();

private slots:
    void onThreadStarted();

private:
    QThread *m_thread = nullptr;
    bool     m_running = false;
};

#endif // QTHREADWORKER_H
