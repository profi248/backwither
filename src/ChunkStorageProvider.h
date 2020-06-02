#ifndef BACKUP_BACKUPSTORAGEPROVIDER_H
#define BACKUP_BACKUPSTORAGEPROVIDER_H
#include "FilesystemEntity.h"
#include "ChunkList.h"
#include <string>
/**
 * Provides a storage to save actual data that is being backed up.
 */

class ChunkStorageProvider {
public:
    virtual ~ChunkStorageProvider() = default;

    virtual size_t StoreChunk (Chunk & metadata, char* data) = 0;
    virtual char* RetrieveChunk (Chunk & metadata) = 0;
};

#endif //BACKUP_BACKUPSTORAGEPROVIDER_H
