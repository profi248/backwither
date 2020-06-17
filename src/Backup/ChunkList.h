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
    /**
     * Consruct chunk list.
     * @param fileId ID of file that chunks belong to.
     */
    ChunkList (int64_t fileId);

    /**
     * Append chunk to storage.
     * @param c Chunk.
     */
    void    AddChunk (Chunk & c);

    /**
     * Get file ID.
     * @return File ID.
     */
    int64_t GetFileID ();

    friend class ChunkListIterator;
};


#endif //BACKWITHER_CHUNKLIST_H
