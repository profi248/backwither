#ifndef BACKWITHER_CHUNK_H
#define BACKWITHER_CHUNK_H
#include <string>
#include <memory>
#include <vector>

class Chunk {
    std::string             m_Hash;
    size_t                  m_Size;
public:
    Chunk (std::string & hash, size_t size);
    std::string GetHash ();
    size_t GetSize ();
};


#endif //BACKWITHER_CHUNK_H
