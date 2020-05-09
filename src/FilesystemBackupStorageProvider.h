#ifndef BACKUP_FILESYSTEMBACKUPSTORAGEPROVIDER_H
#define BACKUP_FILESYSTEMBACKUPSTORAGEPROVIDER_H
#include "FilesystemEntity.h"
#include "BackupStorageProvider.h"
#include <string>
/**
 * Implements BackupStorageProvider as filesystem storage.
 */

class FilesystemBackupStorageProvider : public BackupStorageProvider {
    ~FilesystemBackupStorageProvider() override;
    bool StoreFilesystemEntity(FilesystemEntity e) override;
    virtual FilesystemEntity StoreFilesystemEntity(std::string path);
};

#endif //BACKUP_FILESYSTEMBACKUPSTORAGEPROVIDER_H
