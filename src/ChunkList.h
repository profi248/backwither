#ifndef BACKWITHER_CHUNKLIST_H
#define BACKWITHER_CHUNKLIST_H
#include <string>
#include <vector>
#include "Chunk.h"

/**
 * Represents a list of chunks.
 */

class ChunkList {
    int64_t            m_FileID;
    std::vector<Chunk> m_Chunks;
public:
    ChunkList (int64_t fileId);
    void    AddChunk (Chunk & c);
    int64_t GetFileID ();

    friend class ChunkListIterator;
};


#endif //BACKWITHER_CHUNKLIST_H
