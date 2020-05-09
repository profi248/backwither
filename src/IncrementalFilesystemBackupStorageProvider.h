#ifndef BACKUP_INCREMENTALFILESYSTEMBACKUPSTORAGEPROVIDER_H
#define BACKUP_INCREMENTALFILESYSTEMBACKUPSTORAGEPROVIDER_H
#include "FilesystemEntity.h"
#include "BackupStorageProvider.h"
#include <string>
/**
 * Implements BackupStorageProvider as filesystem storage with incremental backups.
 */

class IncrementalFilesystemBackupStorageProvider : public BackupStorageProvider {
    ~IncrementalFilesystemBackupStorageProvider() override;
    bool StoreFilesystemEntity(FilesystemEntity e) override;
    virtual FilesystemEntity StoreFilesystemEntity(std::string path);
};

#endif //BACKUP_INCREMENTALFILESYSTEMBACKUPSTORAGEPROVIDER_H
