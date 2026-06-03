#include "configuration.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
#include <QtDebug>

static const QString CONFIG_DIR = QStringLiteral("Configuration/");

static QString buildPath(const QString &fn)
{
    QDir dir;
    if (!dir.exists(CONFIG_DIR))
        dir.mkpath(CONFIG_DIR);
    return CONFIG_DIR + fn;
}

Configuration::Configuration()
{
    filename = buildPath(QStringLiteral("config.json"));
    parseJson(readAll());
}

Configuration::Configuration(const QString &fn)
{
    filename = buildPath(fn);
    parseJson(readAll());
}

void Configuration::parseJson(const QByteArray &data)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError)
        qWarning() << "Erreur lors du parsement :" << parseError.offset << ":" << parseError.errorString();
    else {
        qDebug() << "Configuration" << filename << "parsée avec succès";
        m_parsed = doc.object();
    }
}

QByteArray Configuration::readAll()
{
    QFile file(filename);
    bool fileExists = QFileInfo::exists(filename) && QFileInfo(filename).isFile();
    if (fileExists)
        file.open(QIODevice::ReadWrite);
    else
        file.open(QIODevice::NewOnly | QIODevice::ReadWrite);
    return file.readAll();
}

QJsonObject Configuration::getAll() const
{
    return m_parsed;
}

void Configuration::setAll(const QJsonObject &obj)
{
    m_parsed = obj;
    saveToFile();
}

QJsonValue Configuration::get(const QString &key) const
{
    return m_parsed.value(key);
}

void Configuration::set(const QString &key, int value)
{
    m_parsed.insert(key, value);
    saveToFile();
}

void Configuration::set(const QString &key, const QString &value)
{
    m_parsed.insert(key, value);
    saveToFile();
}

void Configuration::saveToFile() const
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Erreur lors de l'écriture du fichier configuration !";
        return;
    }
    file.write(QJsonDocument(m_parsed).toJson());
    file.close();
}
