#ifndef IDATABASEACCESS_H
#define IDATABASEACCESS_H

#include <QList>
#include <QSqlError>
#include <QString>
#include "../../business/models/participant.h"
#include "../../business/models/race.h"

class IDatabaseAccess {
public:
    virtual ~IDatabaseAccess() = default;

    virtual QSqlError initialize() = 0;
    virtual void      close()      = 0;
    virtual bool      isConnected() const = 0;

    virtual bool addParticipant(const Participant &p)    = 0;
    virtual bool updateParticipant(const Participant &p) = 0;
    virtual bool deleteParticipant(int id)               = 0;
    virtual Participant          getParticipant(int id) const        = 0;
    virtual QList<Participant>   getAllParticipants() const           = 0;
    virtual bool participantExists(const QString &email) const       = 0;

    virtual bool addRace(const Race &r)           = 0;
    virtual bool updateRace(const Race &r)        = 0;
    virtual QList<Race> getAllRaces() const        = 0;
    virtual Race        getRace(int id) const     = 0;
};

#endif // IDATABASEACCESS_H
