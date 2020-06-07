#ifndef BACKUP_INCREMENTALFILESYSTEMBACKUPSTORAGEPROVIDER_H
#define BACKUP_INCREMENTALFILESYSTEMBACKUPSTORAGEPROVIDER_H
#include "FilesystemEntity.h"
#include "ChunkStorageProvider.h"
#include "ChunkList.h"
#include <string>
/**
 * Implements ChunkStorageProvider as filesystem storage with incremental backups.
 */

class FilesystemChunkStorageProvider : public ChunkStorageProvider {
    std::string m_OutputPath;
    std::string m_ChunkDir;
public:
    const std::string CHUNK_FOLDER = "chunks/";

    FilesystemChunkStorageProvider (std::string outPath);
    ~FilesystemChunkStorageProvider () override = default;
    size_t StoreChunk (Chunk & metadata, const char* data) override;
    char* RetrieveChunk (Chunk & metadata) override;
};

#endif //BACKUP_INCREMENTALFILESYSTEMBACKUPSTORAGEPROVIDER_H
