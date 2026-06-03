#ifndef DBWORKER_H
#define DBWORKER_H

#include "qthreadworker.h"
#include "../../data/database/idatabaseaccess.h"
#include "../../business/models/participant.h"
#include <QList>

class DatabaseWorker : public QThreadWorker {
    Q_OBJECT
public:
    explicit DatabaseWorker(IDatabaseAccess *db, QObject *parent = nullptr);

    void queryAllParticipants();
    void addParticipant(const Participant &p);
    void updateParticipant(const Participant &p);

signals:
    void participantsLoaded(const QList<Participant> &participants);
    void participantAdded(const Participant &p);
    void queryFailed(const QString &error);

protected:
    void doWork() override;

private:
    enum QueryType { LoadParticipants, AddParticipant, UpdateParticipant };

    IDatabaseAccess *m_db = nullptr;
    QueryType        m_currentQuery = LoadParticipants;
    Participant      m_pendingParticipant;
};

#endif // DBWORKER_H
