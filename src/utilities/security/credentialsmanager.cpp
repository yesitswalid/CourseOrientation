#include "credentialsmanager.h"
#include <QCryptographicHash>
#include <cstdlib>

CredentialsManager &CredentialsManager::instance()
{
    static CredentialsManager inst;
    return inst;
}

bool CredentialsManager::loadCredentials()
{
    m_dbHost     = getEnv("DB_HOST",     "127.0.0.1");
    m_dbUser     = getEnv("DB_USER",     "root");
    m_dbPassword = getEnv("DB_PASSWORD", "");
    m_dbName     = getEnv("DB_NAME",     "");
    return !m_dbName.isEmpty();
}

QString CredentialsManager::getEnv(const QString &key, const QString &defaultValue) const
{
    const char *value = std::getenv(key.toStdString().c_str());
    return value ? QString::fromStdString(value) : defaultValue;
}

QString CredentialsManager::getDatabasePassword() const { return m_dbPassword; }
QString CredentialsManager::getDatabaseUser()     const { return m_dbUser; }
QString CredentialsManager::getDatabaseHost()     const { return m_dbHost; }
QString CredentialsManager::getDatabaseName()     const { return m_dbName; }

QString CredentialsManager::hashPassword(const QString &password)
{
    return QString::fromUtf8(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

bool CredentialsManager::verifyPassword(const QString &password, const QString &hash)
{
    return hashPassword(password) == hash;
}
