#include "ChunkList.h"

ChunkList::ChunkList (std::string & file, int64_t fileId) :
        m_File(file),
        m_FileID(fileId) {}

void ChunkList::AddChunk (Chunk & c) {
    m_Chunks.push_back(c);
}

int64_t ChunkList::GetFileID () {
    return m_FileID;
}
