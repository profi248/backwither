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
public:
    IncrementalFilesystemBackupStorageProvider (std::string outPath);
    ~IncrementalFilesystemBackupStorageProvider () override = default;
    void StoreChunk (Chunk metadata, char* data) override;
    Chunk RetrieveChunk (std::string hash) override;
};

#endif //BACKUP_INCREMENTALFILESYSTEMBACKUPSTORAGEPROVIDER_H
