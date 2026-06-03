#ifndef CREDENTIALSMANAGER_H
#define CREDENTIALSMANAGER_H

#include <QString>

class CredentialsManager {
public:
    static CredentialsManager &instance();

    bool loadCredentials();

    QString getDatabasePassword() const;
    QString getDatabaseUser()     const;
    QString getDatabaseHost()     const;
    QString getDatabaseName()     const;

    static QString hashPassword(const QString &password);
    static bool    verifyPassword(const QString &password, const QString &hash);

private:
    CredentialsManager() = default;
    CredentialsManager(const CredentialsManager &) = delete;
    CredentialsManager &operator=(const CredentialsManager &) = delete;

    QString getEnv(const QString &key, const QString &defaultValue = {}) const;

    QString m_dbPassword;
    QString m_dbUser;
    QString m_dbHost;
    QString m_dbName;
};

#endif // CREDENTIALSMANAGER_H
