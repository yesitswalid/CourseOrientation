#include "application.h"

#include <QApplication>

#include <sqliteconverter.cpp>
#include <QString>
#include <server.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Application w;
    w.show();

    //Initialisation du serveur pour la communication du portique
    Server *server = new Server();

    /*
    QThread *t = new QThread();
    t->setObjectName("Server");
    server->moveToThread(t);
    t->start();

    qDebug() << "Test: " << t->thread()->currentThread();
    qDebug() << "thread Principal:" << QThread::currentThread() << " :" << __LINE__ ;
    */


    return a.exec();
}
