#ifndef BACKWITHER_CHUNKLIST_H
#define BACKWITHER_CHUNKLIST_H
#include <string>
#include <vector>
#include "Chunk.h"

class ChunkList {
    std::string        m_File;
    std::vector<Chunk> m_Chunks;
public:
    ChunkList (std::string & file);
    void AddChunk (Chunk & c);
};


#endif //BACKWITHER_CHUNKLIST_H
