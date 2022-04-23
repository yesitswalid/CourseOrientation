#ifndef CONFIGURATION_CPP
#define CONFIGURATION_CPP

#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>


class Configuration
{

private:
    QString filename;
public:
    QJsonObject m_parsed;
    ~Configuration()
    {

    }
    Configuration()
    {
        init();
    }
    //Création du fichier json avec le nom spécific dans le dossier Configuration
    Configuration(QString fn)
    {
        init(fn);
    }
    //Initialisation et création des report
    void init()
    {

        //Initialisation de la config
        QString path("Configuration/");
        QDir dir;

        filename = path + "config.json";

        //création du dossier Configuration lorsque le dossier n'existe pas.
        if (!dir.exists(path))
        {
            dir.mkpath(path);
        }

        QByteArray values = readAll();
        QTextStream ts(stdout);

          QJsonParseError parseError;
          QJsonDocument doc;
          {
              doc = QJsonDocument::fromJson(values, &parseError);
          }

          if (parseError.error != QJsonParseError::NoError) {
              qWarning() << "Erreur lors du parsement : " << parseError.offset << ":" << parseError.errorString();
          } else {
              qDebug() << "La configuration " + filename + " à bien été parser";
              //ts << doc.toJson(QJsonDocument::Compact);
              m_parsed = doc.object();
          }
    }

    void init(QString fn)
    {

        //Initialisation de la config
        QString path("Configuration/");
        QDir dir;

        filename = path + fn;

        //création du dossier Configuration lorsque le dossier n'existe pas.
        if (!dir.exists(path))
        {
            dir.mkpath(path);
        }

        QByteArray values = readAll();
        QTextStream ts(stdout);

          QJsonParseError parseError;
          QJsonDocument doc;
          {
              doc = QJsonDocument::fromJson(values, &parseError);
          }

          if (parseError.error != QJsonParseError::NoError) {
              qWarning() << "Erreur lors du parsement : " << parseError.offset << ":" << parseError.errorString();
          } else {
              qDebug() << "La configuration " + filename + " à bien été parser";
              ts << doc.toJson(QJsonDocument::Compact);
              m_parsed = doc.object();
          }
    }

    QByteArray readAll()
    {
        QFile file;

        QByteArray value;
        bool fileExists = QFileInfo::exists(filename) && QFileInfo(filename).isFile();
        //Savoir si le fichier config.json ce trouve bien dans le repertoire Configuration/
        //Sinon écrire la nouvelle configuration dans le fichier config.json
        if(fileExists)
        {
            file.setFileName(filename);
            file.open(QIODevice::ReadWrite);
        } else {
            file.setFileName(filename);
            file.open(QIODevice::NewOnly | QIODevice::ReadWrite);
            //file->write("{}");//{'base_de_donnees': {'ip': '127.0.0.1','port': 3306}
        }
       /* qDebug() << "test";
        value = file.readAll();
        m_parsed = value;//QJsonDocument::fromJson(value.toUtf8()).object();
        qDebug() << m_parsed;*/
        value = file.readAll();
        return value;
    }

    //récupération de tous le contenus du format json en object dans la config

    QJsonObject getAll()
    {
        return m_parsed;
    }

    void setAll(QJsonObject obj)
    {
        m_parsed = obj;
        //sauvegarde dans le fichier les objets convertie en json
        QByteArray new_conf = QJsonDocument(m_parsed).toJson();
        QFile file(filename);
        if(!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Erreur lors de l'ecriture du fichier configuration !";
            return;
        }
        file.write(new_conf);
        file.close();
    }


    //récupere la valeur du nom de l'objet

    QJsonValue get(QString key)
    {
        return m_parsed.value(key);
    }


    //Permet d'enregistrer un nouveau objet grace a son nom spécific ainsi que sa valeur en nombre entier

    void set(QString key, int value)
    {
        QJsonObject *obj;
        obj = &m_parsed;
        obj->insert(key, value);
        m_parsed = *obj;
        //sauvegarde dans le fichier les objets convertie en json
        QByteArray new_conf = QJsonDocument(m_parsed).toJson();

        QFile file(filename);
        if(!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Erreur lors de l'ecriture du fichier configuration !";
            return;
        }
        file.write(new_conf);
        file.close();
    }

    //Permet d'enregistrer un nouveau objet grace a son nom spécific ainsi que sa valeur qui est une chaine de caractere contrairement à la fonction prècedente

    void set(QString key, QString value)
    {
        QJsonObject *obj;
        obj = &m_parsed;
        obj->insert(key, value);
        m_parsed = *obj;
        //sauvegarde dans le fichier les objets convertie en json
        QByteArray new_conf = QJsonDocument(m_parsed).toJson();

        QFile file(filename);
        if(!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Erreur lors de l'ecriture du fichier configuration !";
            return;
        }
        file.write(new_conf);
        file.close();
    }
};
#endif
