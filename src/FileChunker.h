#ifndef BACKWITHER_FILECHUNKER_H
#define BACKWITHER_FILECHUNKER_H
#include <string>
#include "ConfigProvider.h"

class FileChunker {
    static constexpr int CHUNK_SIZE = 1024 * 1024; // 1 MiB
    static char* buf; // make it static class member to avoid memory leaks when exception is thrown
public:
    static constexpr int MAX_CHUNK_SIZE = CHUNK_SIZE;
    // todo refactor config provider to member var
    static void SaveFileChunks (std::string inFile, int64_t fileID, std::string outFolder, int64_t snapshotId,
                                ConfigProvider* config, bool compressed);
    ~FileChunker ();

private:
    static std::string chunkHashSha256(char* data, size_t size);
};



#endif //BACKWITHER_FILECHUNKER_H
