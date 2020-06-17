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
    /**
     * Construct snapshot.
     * @param id Snapshot ID.
     * @param created Unix timestamp of creation.
     * @param completed Unix timestamp of completion.
     */
    Snapshot (int64_t id, long long created, long long completed);

    /**
     * Get snapshot ID.
     * @return Snapshot ID.
     */
    int64_t GetID () const;

    /**
     * Get Unix timestamp of creation.
     * @return Unix timestamp of creation.
     */
    long long GetCreation () const;

    /**
     * Get Unix timestamp of completion.
     * @return Unix timestamp of completion.
     */
    long long GetCompletion () const;
};


#endif //BACKWITHER_SNAPSHOT_H
