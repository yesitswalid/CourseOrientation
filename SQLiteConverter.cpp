#include <QMap>
#include <QString>

class SQLiteConverter{

SQLiteConverter(){
    //Constructeur
}

~SQLiteConverter()
{
    //Destructeur
}

QMap<QString, QString> SqlDataToMap()
{
    QMap<QString, QString>parsed;
    parsed["test"] = "d";
    return parsed;
}

};
