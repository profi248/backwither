#include "Chunk.h"

Chunk::Chunk (std::string & hash, size_t size) :
        m_Hash (hash),
        m_Size (size) {}

std::string Chunk::GetHash () {
    return m_Hash;
}

size_t Chunk::GetSize () {
    return m_Size;
}

