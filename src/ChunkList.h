#ifndef BACKWITHER_CHUNKLIST_H
#define BACKWITHER_CHUNKLIST_H
#include <string>
#include <vector>
#include "Chunk.h"

class ChunkList {
    std::string        m_File;
    int64_t            m_FileID;
    std::vector<Chunk> m_Chunks;
public:
    ChunkList (std::string & file, int64_t fileId);
    void    AddChunk (Chunk & c);
    int64_t GetFileID ();

    friend class ChunkListIterator;
};


#endif //BACKWITHER_CHUNKLIST_H
