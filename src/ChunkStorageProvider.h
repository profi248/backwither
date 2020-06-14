#ifndef BACKUP_CHUNKSTORAGEPROVIDER_H
#define BACKUP_CHUNKSTORAGEPROVIDER_H
#include "FilesystemEntity.h"
#include "ChunkList.h"
#include <string>
/**
 * Provides a storage to save actual data that is being backed up.
 */

class ChunkStorageProvider {
public:
    virtual ~ChunkStorageProvider() = default;

    /**
     * Save a chunk to storage.
     *
     * @param metadata Chunk metadata (including correct size).
     * @param data Actual raw data.
     * @return New chunk size after storage (it may change due to compression).
     */
    virtual size_t StoreChunk (Chunk & metadata, const char* data) = 0;

    /**
     * Retrieve a chunk from the storage.
     * @param metadata Chunk metadata, size will be updated if it changes after retrieval (due to decompression).
     * @return Actual raw data.
     */
    virtual char* RetrieveChunk (Chunk & metadata) = 0;
};

#endif //BACKUP_CHUNKSTORAGEPROVIDER_H
