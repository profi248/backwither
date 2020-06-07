#ifndef BACKWITHER_COMPRESSEDFILESYSTEMCHUNKSTORAGEPROVIDER_H
#define BACKWITHER_COMPRESSEDFILESYSTEMCHUNKSTORAGEPROVIDER_H
#include "FilesystemEntity.h"
#include "ChunkStorageProvider.h"
#include "ChunkList.h"
#include "FilesystemChunkStorageProvider.h"
#include <string>

class CompressedFilesystemChunkStorageProvider : public FilesystemChunkStorageProvider {
    std::string m_OutputPath;
    std::string m_ChunkDir;
public:
    const std::string CHUNK_FOLDER = "chunks/";

    explicit CompressedFilesystemChunkStorageProvider (std::string outPath);
    ~CompressedFilesystemChunkStorageProvider () override = default;
    size_t StoreChunk (Chunk & metadata, const char* data) override;
    char* RetrieveChunk (Chunk & metadata) override;
};



#endif //BACKWITHER_COMPRESSEDFILESYSTEMCHUNKSTORAGEPROVIDER_H
