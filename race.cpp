#include "race.h"

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
Race::Race()
{

}
