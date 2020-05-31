#ifndef BACKUP_INCREMENTALFILESYSTEMBACKUPSTORAGEPROVIDER_H
#define BACKUP_INCREMENTALFILESYSTEMBACKUPSTORAGEPROVIDER_H
#include "FilesystemEntity.h"
#include "BackupStorageProvider.h"
#include "ChunkList.h"
#include <string>
/**
 * Implements BackupStorageProvider as filesystem storage with incremental backups.
 */

class IncrementalFilesystemBackupStorageProvider : public BackupStorageProvider {
    const std::string CHUNK_FOLDER = "chunks/";

    std::string m_OutputPath;
    std::string m_ChunkDir;
    char*       m_Buf = nullptr;
public:
    IncrementalFilesystemBackupStorageProvider (std::string outPath);
    ~IncrementalFilesystemBackupStorageProvider () override;
    void StoreChunk (Chunk metadata, char* data) override;
    char* RetrieveChunk (Chunk metadata) override;
};

#endif //BACKUP_INCREMENTALFILESYSTEMBACKUPSTORAGEPROVIDER_H
