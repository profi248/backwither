#include <zlib.h>
#include <stdexcept>
#include <iostream>
#include "CompressedFilesystemChunkStorageProvider.h"
#include "FileChunker.h"

size_t CompressedFilesystemChunkStorageProvider::StoreChunk (Chunk & metadata, char* data) {
    size_t compressedSize(compressBound(metadata.GetSize()));
    char* compressed = new char[compressedSize];
    int result = compress2(reinterpret_cast<Bytef*>(compressed), reinterpret_cast<uLongf*>(& compressedSize),
                           reinterpret_cast<const Bytef*>(data), metadata.GetSize(), Z_DEFAULT_COMPRESSION);
    if (result != Z_OK) {
        delete [] compressed;
        delete [] data;
        throw std::runtime_error("Error when compressing a chunk.");
    }

    metadata.SetSize(compressedSize);
    size_t newSize = FilesystemChunkStorageProvider::StoreChunk(metadata, compressed);
    delete [] compressed;
    return newSize;
}

char* CompressedFilesystemChunkStorageProvider::RetrieveChunk (Chunk & metadata) {
    char* compressed = FilesystemChunkStorageProvider::RetrieveChunk(metadata);
    size_t uncompresedSize = FileChunker::MAX_CHUNK_SIZE;
    char* data = new char[uncompresedSize];
    int result = uncompress(reinterpret_cast<Bytef*>(data), reinterpret_cast<uLongf*>(& uncompresedSize),
                            reinterpret_cast<const Bytef*>(compressed), metadata.GetSize());
    if (result != Z_OK) {
        delete [] compressed;
        delete [] data;
        throw std::runtime_error("Error when decompressing a chunk.");
    }
    metadata.SetSize(uncompresedSize);
    delete [] compressed;
    return data;
}

CompressedFilesystemChunkStorageProvider::CompressedFilesystemChunkStorageProvider (std::string outPath) : FilesystemChunkStorageProvider(outPath) {}
