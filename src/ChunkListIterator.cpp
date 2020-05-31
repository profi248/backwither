#include "ChunkListIterator.h"

ChunkListIterator::ChunkListIterator (ChunkList * chunks) :
        m_ChunkList (chunks),
        m_Pos (0) {}

void ChunkListIterator::Next () {
    m_Pos++;
}

bool ChunkListIterator::End () const {
    return m_Pos >= m_ChunkList->m_Chunks.size();
}

bool ChunkListIterator::Empty () const {
    return m_ChunkList->m_Chunks.empty();
}

std::string ChunkListIterator::GetHash () const {
    return m_ChunkList->m_Chunks[m_Pos].GetHash();
}

size_t ChunkListIterator::GetSize () const {
    return m_ChunkList->m_Chunks[m_Pos].GetSize();
}

ChunkListIterator ChunkListIterator::operator ++ (int) {
    Next();
    return *this;
}
