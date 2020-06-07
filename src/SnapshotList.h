#ifndef BACKWITHER_SNAPSHOTLIST_H
#define BACKWITHER_SNAPSHOTLIST_H
#include <vector>
#include "Snapshot.h"

class SnapshotListIterator;

class SnapshotList {
    std::vector<Snapshot> m_Snapshots;
public:
    void AddSnapshot(Snapshot s);
    friend class SnapshotListIterator;
};


#endif //BACKWITHER_SNAPSHOTLIST_H
