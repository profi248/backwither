#ifndef BACKWITHER_COMPRESSEDFILESYSTEMCHUNKSTORAGEPROVIDER_H
#define BACKWITHER_COMPRESSEDFILESYSTEMCHUNKSTORAGEPROVIDER_H
#include "FilesystemEntity.h"
#include "ChunkStorageProvider.h"
#include "ChunkList.h"
#include "FilesystemChunkStorageProvider.h"
#include <zlib.h>
#include <string>

/**
 * Implements ChunkStorageProvider as filesystem storage with zlib (deflate) compression of each chunk.
 */

class CompressedFilesystemChunkStorageProvider : public FilesystemChunkStorageProvider {
    std::string m_OutputPath;
    std::string m_ChunkDir;
    int         m_CompressionLevel = Z_DEFAULT_COMPRESSION;
public:
    const std::string CHUNK_FOLDER = "chunks/";

    explicit CompressedFilesystemChunkStorageProvider (std::string outPath);
    ~CompressedFilesystemChunkStorageProvider () override = default;

    [[nodiscard]] size_t StoreChunk (Chunk & metadata, const char* data) override;
    char*  RetrieveChunk (Chunk & metadata) override;
};



#endif //BACKWITHER_COMPRESSEDFILESYSTEMCHUNKSTORAGEPROVIDER_H
