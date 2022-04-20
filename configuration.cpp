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
    static QJsonDocument m_parsed;

public:
    Configuration();
    //Initialisation de la config
    static void init()
    {
        //Initialisation de la config
        QFile file;
        QString path("Configuration/");
        QDir dir;
        QString value;

        //création du dossier Configuration lorsque le dossier n'existe pas.
        if (!dir.exists(path))
            dir.mkpath(path);

        bool fileExists = QFileInfo::exists(path + "config.json") && QFileInfo(path + "config.json").isFile();
        //Savoir si le fichier config.json ce trouve bien dans le repertoire Configuration/
        //Sinon écrire la nouvelle configuration dans le fichier config.json
        if(fileExists)
        {
            file.setFileName(path + "config.json");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
        } else {
            file.setFileName(path + "config.json");
            file.open(QIODevice::NewOnly | QIODevice::ReadWrite);
            file.write("{'base_de_donnees': {'ip': '127.0.0.1','port': 3306}");
        }
        value = file.readAll();
        m_parsed = QJsonDocument::fromJson(value.toUtf8());
        qDebug() << m_parsed;
        file.close();

    }

    //récupération de tous le contenus du format json en object dans la config

    static QJsonObject getAll()
    {
        return m_parsed.object();
    }


    //récupere la valeur du nom de l'objet

    static QJsonValue get(QString key)
    {
        return m_parsed.object().value(key);
    }


    //Permet d'enregistrer un nouveau objet grace a son nom spécific ainsi que sa valeur en nombre entier

    static void set(QString key, int value)
    {
        QJsonObject obj = m_parsed.object();
        obj[key] = value;
        m_parsed = QJsonDocument::fromVariant(obj.toVariantMap());
    }

    //Permet d'enregistrer un nouveau objet grace a son nom spécific ainsi que sa valeur qui est une chaine de caractere contrairement à la fonction prècedente

    static void set(QString key, QString value)
    {
        QJsonObject obj = m_parsed.object();
        obj[key] = value;
        m_parsed = QJsonDocument::fromVariant(obj.toVariantMap());
    }
};


