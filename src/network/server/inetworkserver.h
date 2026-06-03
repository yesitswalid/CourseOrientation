#ifndef INETWORKSERVER_H
#define INETWORKSERVER_H

#include <QObject>
#include <QString>
#include <QByteArray>

class INetworkServer : public QObject {
    Q_OBJECT
public:
    virtual ~INetworkServer() = default;

    virtual bool start()        = 0;
    virtual void stop()         = 0;
    virtual bool isRunning() const = 0;

signals:
    void cardIdReceived(const QString &cardId);
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &error);
};

#endif // INETWORKSERVER_H
