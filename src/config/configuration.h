#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QJsonObject>
#include <QJsonValue>

class Configuration
{
public:
    Configuration();
    explicit Configuration(const QString &fn);
    ~Configuration() = default;

    QByteArray readAll();
    QJsonObject getAll() const;
    void setAll(const QJsonObject &obj);
    QJsonValue get(const QString &key) const;
    void set(const QString &key, int value);
    void set(const QString &key, const QString &value);

private:
    void saveToFile() const;
    void parseJson(const QByteArray &data);

    QString filename;
    QJsonObject m_parsed;
};

#endif // CONFIGURATION_H
