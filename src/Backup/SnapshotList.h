#ifndef BACKWITHER_SNAPSHOTLIST_H
#define BACKWITHER_SNAPSHOTLIST_H
#include <vector>
#include "Snapshot.h"

/**
 * Represents a list of snapshots.
 */

class SnapshotListIterator;

class SnapshotList {
    std::vector<Snapshot> m_Snapshots;
public:
    /**
     * Append snapshot to list.
     * @param s Snapshot to append.
     */
    void AddSnapshot(Snapshot s);
    friend class SnapshotListIterator;
};


#endif //BACKWITHER_SNAPSHOTLIST_H
