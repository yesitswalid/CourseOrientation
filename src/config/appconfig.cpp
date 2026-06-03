#include "appconfig.h"
#include <QFile>
#include <QJsonDocument>
#include <cstdlib>

AppConfig &AppConfig::instance()
{
    static AppConfig inst;
    return inst;
}

bool AppConfig::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    m_config = doc.object();
    return true;
}

bool AppConfig::loadFromEnvironment()
{
    return true;
}

QString AppConfig::getEnv(const QString &key, const QString &defaultValue) const
{
    const char *value = std::getenv(key.toStdString().c_str());
    return value ? QString::fromStdString(value) : defaultValue;
}

QString AppConfig::dbHost() const
{
    return getEnv("DB_HOST", m_config["database"].toObject()["host"].toString("127.0.0.1"));
}

int AppConfig::dbPort() const
{
    bool ok = false;
    int envPort = getEnv("DB_PORT").toInt(&ok);
    if (ok) return envPort;
    return m_config["database"].toObject()["port"].toInt(3306);
}

QString AppConfig::dbUser() const
{
    return getEnv("DB_USER", m_config["database"].toObject()["user"].toString("root"));
}

QString AppConfig::dbPassword() const
{
    return getEnv("DB_PASSWORD", m_config["database"].toObject()["password"].toString());
}

QString AppConfig::dbName() const
{
    return getEnv("DB_NAME", m_config["database"].toObject()["name"].toString());
}

QString AppConfig::dbLocalPath() const
{
    return getEnv("DB_LOCAL_PATH", m_config["database"].toObject()["localPath"].toString("course.db"));
}

int AppConfig::networkPort() const
{
    bool ok = false;
    int envPort = getEnv("NETWORK_PORT").toInt(&ok);
    if (ok) return envPort;
    return m_config["network"].toObject()["port"].toInt(1234);
}

QString AppConfig::networkHost() const
{
    return getEnv("NETWORK_HOST", m_config["network"].toObject()["host"].toString("0.0.0.0"));
}

int AppConfig::networkTimeout() const
{
    bool ok = false;
    int envTimeout = getEnv("NETWORK_TIMEOUT").toInt(&ok);
    if (ok) return envTimeout;
    return m_config["network"].toObject()["timeout"].toInt(5000);
}

QString AppConfig::logLevel() const
{
    return getEnv("LOG_LEVEL", m_config["logging"].toObject()["level"].toString("info"));
}

QString AppConfig::logFilePath() const
{
    return getEnv("LOG_FILE", m_config["logging"].toObject()["filePath"].toString("app.log"));
}
