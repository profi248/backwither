#ifndef BACKUP_FILESYSTEMBACKUPSTORAGEPROVIDER_H
#define BACKUP_FILESYSTEMBACKUPSTORAGEPROVIDER_H
#include "FilesystemEntity.h"
#include "BackupStorageProvider.h"
#include "ChunkList.h"
#include <string>
/**
 * Implements BackupStorageProvider as filesystem storage.
 */

class FilesystemBackupStorageProvider : public BackupStorageProvider {
    ~FilesystemBackupStorageProvider() override = default;
    void StoreChunk (Chunk metadata, char* data) override;
    virtual char* RetrieveChunk (Chunk metadata);
};

#endif //BACKUP_FILESYSTEMBACKUPSTORAGEPROVIDER_H
