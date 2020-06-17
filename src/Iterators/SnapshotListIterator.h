#ifndef BACKWITHER_SNAPSHOTLISTITERATOR_H
#define BACKWITHER_SNAPSHOTLISTITERATOR_H
#include "../Backup/SnapshotList.h"
#include "SimpleIterator.h"

/**
 * Iterate over snapshots in a backup.
 */

class SnapshotListIterator : public SimpleIterator {
    SnapshotList * m_SnapshotList;
    size_t         m_Pos;

public:
    explicit  SnapshotListIterator (SnapshotList * list);

    void      Next () override;
    void      Rewind () override;
    int64_t   GetID () const;
    long long GetCreation () const;
    long long GetCompletion () const;
    bool      End () const override;
    bool      Empty () const override;

    std::vector<std::string> TableHeader () const override;
    std::vector<std::string> TableRow () const override;

    void operator ++ (int) override;
};

#endif //BACKWITHER_SNAPSHOTLISTITERATOR_H
