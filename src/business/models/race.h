#ifndef RACE_H
#define RACE_H

#include <QString>
#include <QDateTime>

struct Race {
    int      id           = -1;
    int      departmentId = -1;
    QString  name;
    QDateTime date;
    QString  location;
    float    gpsLongitude = 0.0f;
    float    gpsLatitude  = 0.0f;
    int      difficulty   = 0;
    int      type         = 0;
    int      book         = 0;

    bool isValid() const;
};

inline bool Race::isValid() const
{
    return id >= 0 && !name.isEmpty();
}

#endif // RACE_H
