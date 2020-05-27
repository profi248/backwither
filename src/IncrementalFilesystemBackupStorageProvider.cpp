#include <filesystem>
#include <stdexcept>
#include <fstream>
#include "IncrementalFilesystemBackupStorageProvider.h"
#include "FilesystemBrowser.h"


using namespace std;
namespace fs = std::filesystem;

IncrementalFilesystemBackupStorageProvider::IncrementalFilesystemBackupStorageProvider (std::string out) :
    m_OutputPath(out)
{
    m_ChunkDir = FilesystemBrowser::NormalizeDirectoryPath(m_OutputPath) + CHUNK_FOLDER;
    fs::create_directories(m_ChunkDir);
    if (!fs::exists(m_ChunkDir))
        throw runtime_error("Cannot create directory " + m_ChunkDir + ".");
}

void IncrementalFilesystemBackupStorageProvider::StoreChunk (Chunk metadata, char* data) {
    string chunkFile = m_ChunkDir + metadata.GetHash();
    if (!fs::exists(chunkFile)) {
        fstream file (chunkFile, ios::out | ios::binary);
        file.write(data, metadata.GetSize());
        if (!file.good())
            throw runtime_error("Cannot write in " + chunkFile + ".");
    }
}

Chunk IncrementalFilesystemBackupStorageProvider::RetrieveChunk (std::string hash) {

}
