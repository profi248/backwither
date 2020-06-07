#ifndef BACKWITHER_SNAPSHOT_H
#define BACKWITHER_SNAPSHOT_H
#include <cstdint>

// todo save additional data like size difference / file difference
class Snapshot {
    int64_t   m_ID;
    long long m_Created;
public:
    Snapshot (int64_t id, long long created);

    int64_t   GetID () const;
    long long GetCreation () const;
};


#endif //BACKWITHER_SNAPSHOT_H
