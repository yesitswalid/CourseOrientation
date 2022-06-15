#ifndef RACEMANAGER_H
#define RACEMANAGER_H

#include <string.h>
#include <iostream>
#include <QString>
#include <QVector>


class RaceManager
{



public:
    const static int DEFAULT = 0;
    const static int RAZ = 1;
    const static int DATA = 2;

    struct Race
    {
            QString name;
            int raceId;
            QString date;
            QString location;
            int departmentId;
            float gpsLongitude;
            float gpsLatitude;
            int difficulty;
            int type;
            int book;
    };

    static RaceManager *getInstance();
    RaceManager();

    void setMode(const int mode);
    int getMode();

    bool isRaceSelected();
    void setRaceSelected(bool isSelected);
    QVector<Race> getRaces();
    void setRaces(QVector<Race> races);
    void setRace(int raceId,
                 int departmentId,
                 QString name,
                 QString date,
                 QString location,
                 float gpsLongitude,
                 float gpsLatitude,
                 int difficulty,
                 int type,
                 int book);

    QString getName();
    void setName(QString name);

    int getRaceId();
    void setRaceId(int raceId);

    QString getDate();
    void setDate(QString date);

    QString getLocation();
    void setLocation(QString location);

    int getDepartmentId();
    void setDepartmentId(int departmentId);

    float getGPSLongitude();
    void setGPSLongitude(float gpsLongitude);

    float getGPSLatitude();
    void setGPSLatitude(float gpsLatitude);

    int getDifficulty();
    void setDifficulty(int difficulty);

    int getType();
    void setType(int type);

    int getBook();
    void setBook(int book);
    //static Race *instance;

protected:
        static RaceManager *instance;

private:
    bool raceSelected = false;
    int mode = DEFAULT; //par defaut le mode est à DEFAULT
    QVector<Race> races;
    QString name;
    int raceId;
    QString date;
    QString location;
    int departmentId;
    float gpsLongitude;
    float gpsLatitude;
    int difficulty;
    int type;
    int book;

};

#endif // RACEMANAGER_H
