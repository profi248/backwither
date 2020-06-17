#ifndef BACKWITHER_CHUNKLISTITERATOR_H
#define BACKWITHER_CHUNKLISTITERATOR_H
#include "../Backup/ChunkList.h"

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

    /**
     * Get current hash.
     * @return Hexadecimal chunk hash.
     */
    std::string GetHash () const;

    /**
     * Get current size.
     * @return Current chunk size in bytes.
     */
    size_t      GetSize () const;

    /**
     * Get current Chunk object.
     * @return Chunk object.
     */
    Chunk       Current () const;
    bool        End () const;
    bool        Empty () const;

    ChunkListIterator operator ++ (int);
};

#endif //BACKWITHER_ChunkListIterator_H
