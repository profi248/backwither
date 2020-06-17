#ifndef BACKWITHER_SNAPSHOT_H
#define BACKWITHER_SNAPSHOT_H
#include <cstdint>

/**
 * Represents metadata of a snapshot.
 */

class Snapshot {
    int64_t   m_ID;
    long long m_Created;
    long long m_Completed;
public:
    Snapshot (int64_t id, long long created, long long completed);

    int64_t   GetID () const;
    long long GetCreation () const;
    long long GetCompletion () const;
};


#endif //BACKWITHER_SNAPSHOT_H
