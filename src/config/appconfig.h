#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>
#include <QJsonObject>

class AppConfig {
public:
    static AppConfig &instance();

    bool loadFromFile(const QString &filePath);
    bool loadFromEnvironment();

    QString dbHost()      const;
    int     dbPort()      const;
    QString dbUser()      const;
    QString dbPassword()  const;
    QString dbName()      const;
    QString dbLocalPath() const;

    int     networkPort()    const;
    QString networkHost()    const;
    int     networkTimeout() const;

    QString logLevel()    const;
    QString logFilePath() const;

private:
    AppConfig() = default;
    AppConfig(const AppConfig &) = delete;
    AppConfig &operator=(const AppConfig &) = delete;

    QString getEnv(const QString &key, const QString &defaultValue = {}) const;

    QJsonObject m_config;
};

#endif // APPCONFIG_H
