#ifndef BACKWITHER_CHUNK_H
#define BACKWITHER_CHUNK_H
#include <string>
#include <memory>
#include <vector>

/**
 * Represents a chunk (storing metadata).
 */

class Chunk {
    std::string m_Hash;
    size_t      m_Size;
public:
    Chunk (std::string hash, size_t size);

    /**
     * Get chunk hash (hexadecimal ASCII).
     * @return Hash.
     */
    std::string GetHash ();

    /**
     * Get chunk size in bytes.
     * @return Size.
     */
    size_t      GetSize ();

    /**
     * Set chunk size in bytes.
     * @param size Size.
     */
    void        SetSize (size_t size);
};


#endif //BACKWITHER_CHUNK_H
