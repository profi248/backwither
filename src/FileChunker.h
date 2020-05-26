#ifndef BACKWITHER_FILECHUNKER_H
#define BACKWITHER_FILECHUNKER_H
#include <string>

class FileChunker {
    static constexpr int CHUNK_SIZE = 1024 * 1024; // 1 MiB
public:
    static void GenerateFileChunks (std::string path);

private:
    static std::string chunkHashSha256(char* data, size_t size);
};


#endif //BACKWITHER_FILECHUNKER_H
