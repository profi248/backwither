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
    const std::string CHUNK_FOLDER = "chunks/";

    std::string m_OutputPath;
    std::string m_ChunkDir;
    char*       m_Buf = nullptr;
public:
    FilesystemChunkStorageProvider (std::string outPath);
    ~FilesystemChunkStorageProvider () override;
    void  StoreChunk (Chunk metadata, char* data) override;
    char* RetrieveChunk (Chunk metadata) override;
};

#endif //BACKUP_INCREMENTALFILESYSTEMBACKUPSTORAGEPROVIDER_H
