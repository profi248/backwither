#ifndef BACKWITHER_CHUNKLISTITERATOR_H
#define BACKWITHER_CHUNKLISTITERATOR_H
#include "ChunkList.h"

class BackupPlan;

/**
 * Allow iterating over chunks of a file.
 */

class ChunkListIterator {
    ChunkList * m_ChunkList;
    size_t      m_Pos;
public:
    ChunkListIterator (ChunkList * chunks);
    ~ChunkListIterator () = default;

    void        Next ();
    std::string GetHash () const;
    size_t      GetSize () const;
    Chunk       Current () const;
    bool        End () const;
    bool        Empty () const;

    ChunkListIterator operator ++ (int);
};

#endif //BACKWITHER_ChunkListIterator_H
