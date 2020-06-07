#ifndef BACKWITHER_SNAPSHOTLISTITERATOR_H
#define BACKWITHER_SNAPSHOTLISTITERATOR_H
#include "SnapshotList.h"

class SnapshotListIterator {
    SnapshotList * m_SnapshotList;
    size_t         m_Pos;

public:
    explicit SnapshotListIterator (SnapshotList * list);

    void      Next ();
    void      Rewind ();
    int64_t   GetID () const;
    long long GetCreation () const;
    long long GetCompletion () const;
    bool      End () const;
    bool      Empty () const;

    SnapshotListIterator operator ++ (int);
};

#endif //BACKWITHER_SNAPSHOTLISTITERATOR_H
