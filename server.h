#include <QDebug>
#include <QThread>
#include <QUdpSocket>

#define PORT 1234

class Server : public QObject
{
    Q_OBJECT
private:
    QUdpSocket * socket;
public:
    Server(QObject* parent = 0) : QObject(parent)
    {
        init();
    }
    void init()
    {
        socket = new QUdpSocket();
        socket->bind(PORT);
        socket->connect(socket, SIGNAL(readyRead()),this,SLOT(readPendingDiagrams()));
        qDebug() << "Attente de données depuis le port: " << PORT << " ... \n";
    }


public slots:
    void readPendingDiagrams()
    {
        qDebug() << "Lire depuis le thread:" << QThread::currentThread();

        QByteArray buffer;

        buffer.resize(socket->pendingDatagramSize());
        QHostAddress sender;

        quint16 senderPort;

        socket->readDatagram(buffer.data(), buffer.size(),
                             &sender, &senderPort);

        qDebug() << "Client from: " << sender.toString();
        qDebug() << "Client port: " << senderPort;
        qDebug() << "Buffer: " << buffer;

        //Pour supprimer l'ancienne données et recuperer l'id
        if(buffer.contains("INIT"))
        {

            socket->writeDatagram(QByteArray("R"), sender, senderPort);
            //clientPortique = sender, senderPort

            //PORTIQUE_DATA (Recupere les données de balises)
        }else if(buffer.contains("test"))
        {
            //récuperer les données du participant course
            //Récupération de la données
                        socket->writeDatagram(QByteArray("Numero serie: J388B"), sender, senderPort);
        }
       //char *mess = "Salut venant du serveur";

    }
};
