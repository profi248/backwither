#ifndef BACKWITHER_FILECHUNKER_H
#define BACKWITHER_FILECHUNKER_H
#include <string>
#include "ConfigProvider.h"
#include "BackupIndexProvider.h"

class FileChunker {
    /// constant size of chunk
    static constexpr int CHUNK_SIZE = 1024 * 1024; // 1 MiB
    /// temporary data
    static char* buf; // make it static class member to avoid memory leaks when exception is thrown
public:
    /**
     * Maximum possible chunk size (e.g. for allocation).
     */
    static constexpr int MAX_CHUNK_SIZE = CHUNK_SIZE;
    ~FileChunker ();

    /**
     * Splits inputted file into chunks for deduplication.
     * Chunk index is then saved by BackupIndexProvider and data of each chunks is saved by ChunkStorageProvider.
     *
     * @param inFile Absolute path to input file.
     * @param fileID ID of input file.
     * @param outFolder Absolute path to output folder.
     * @param snapshotId Snapshot ID to save.
     * @param config BackupIndexProvider instance.
     * @param compressed Whether file should be compressed.
     */
    static void SaveFileChunks (std::string inFile, int64_t fileID, std::string outFolder, int64_t snapshotId,
                                std::unique_ptr<BackupIndexProvider> & config, bool compressed);

    /**
     * Pieces chunks back into files, from source to filePath.
     * Data is retrieved from ChunkStorageProvider.
     *
     * @param source Absolute path to folder with chunks.
     * @param chunks List of chunks from BackupIndexProvider.
     * @param filePath Absoulute path to file.
     * @param compressed Whether chunks are compressed.
     */
    static void RestoreFileFromChunks (std::string source, ChunkList chunks, std::string filePath, bool compressed);

    /**
     * Computes SHA-256 hash of a chunk.
     *
     * @param data Raw data.
     * @param size Size of raw data.
     * @return Hash in hex form.
     */
    static std::string ChunkHashSha256(const char* data, size_t size);
};



#endif //BACKWITHER_FILECHUNKER_H
