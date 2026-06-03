#ifndef SQLITECONVERTER_H
#define SQLITECONVERTER_H

#include <QSqlDatabase>
#include <QString>

class SQLiteConverter
{
public:
    SQLiteConverter();
    explicit SQLiteConverter(const QString &dbPath);
    ~SQLiteConverter();

    bool exportSql();
    QSqlDatabase *getDatabase();
    void setPath(const QString &path);
    QString getPath() const;

private:
    QSqlDatabase *m_db = nullptr;
    QString path;
};

#endif // SQLITECONVERTER_H
