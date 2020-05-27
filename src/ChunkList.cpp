#include "ChunkList.h"

ChunkList::ChunkList (std::string & file) :
        m_File(file) {}

void ChunkList::AddChunk (Chunk & c) {
    m_Chunks.push_back(c);
}
