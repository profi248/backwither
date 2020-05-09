#ifndef BACKUP_BACKUPSTORAGEPROVIDER_H
#define BACKUP_BACKUPSTORAGEPROVIDER_H
#include "FilesystemEntity.h"
#include <string>
/**
 * Provides a storage to save actual data that is being backed up.
 */

class BackupStorageProvider {
    virtual ~BackupStorageProvider() = default;
    /**
     * Save a FilesystemEntity to the storage.
     * @param e FilesystemEntity object.
     * @return Success.
     */
    virtual bool StoreFilesystemEntity(FilesystemEntity e);
    /**
     * Retrieve a FilesystemEntity from the storage.
     * @param path FilesystemEntity path.
     * @return The FilesystemEntity.
     */
    virtual FilesystemEntity StoreFilesystemEntity(std::string path);
};

#endif //BACKUP_BACKUPSTORAGEPROVIDER_H
