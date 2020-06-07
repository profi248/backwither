#include <filesystem>
#include <fstream>
#include "FilesystemChunkStorageProvider.h"
#include "FilesystemUtils.h"


using namespace std;
namespace fs = std::filesystem;

FilesystemChunkStorageProvider::FilesystemChunkStorageProvider (std::string out) :
    m_OutputPath(out)
{
    m_ChunkDir = FilesystemUtils::NormalizeDirectoryPath(m_OutputPath) + CHUNK_FOLDER;
    fs::create_directories(m_ChunkDir);
    if (!fs::exists(m_ChunkDir))
        throw runtime_error("Cannot create directory " + m_ChunkDir + ".");
}

size_t FilesystemChunkStorageProvider::StoreChunk (Chunk & metadata, char* data) {
    string chunkFile = m_ChunkDir + metadata.GetHash();
    if (!fs::exists(chunkFile)) {
        fstream file (chunkFile, ios::out | ios::binary);
        file.write(data, metadata.GetSize());
        if (!file.good())
            throw runtime_error("Cannot write in " + chunkFile + ".");
    }

    return metadata.GetSize();
}

char* FilesystemChunkStorageProvider::RetrieveChunk (Chunk & metadata) {
    string chunkFile = m_ChunkDir + metadata.GetHash();
    char* buf;
    if (fs::exists(chunkFile)) {
        fstream file (chunkFile, ios::in | ios::binary);
        buf = new char [metadata.GetSize()];
        file.read(buf, metadata.GetSize());
        if (!file.good())
            throw runtime_error("Cannot read chunk in " + chunkFile + ".");
    } else {
        throw runtime_error("Chunk in " + chunkFile + " does not exist.");
    }
    return buf;
}
