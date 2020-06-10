#include <filesystem>
#include <openssl/sha.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "FileChunker.h"
#include "FilesystemChunkStorageProvider.h"
#include "CompressedFilesystemChunkStorageProvider.h"
#include "ChunkListIterator.h"
#include "FilesystemUtils.h"

using namespace std;
namespace fs = filesystem;

char* FileChunker::buf = nullptr;

void FileChunker::SaveFileChunks (std::string inFile, int64_t fileID, std::string outFolder, int64_t snapshotId,
                                  unique_ptr<BackupIndexProvider> & config, bool compressed) {
    fstream file = fstream(inFile, ios::in | ios::binary);
    size_t chunkCnt = 0;

    unique_ptr<ChunkStorageProvider> storageProvider;

    if (compressed)
        storageProvider = make_unique<CompressedFilesystemChunkStorageProvider>
                                          (CompressedFilesystemChunkStorageProvider(outFolder));
    else
        storageProvider = make_unique<FilesystemChunkStorageProvider>
                                          (FilesystemChunkStorageProvider(outFolder));

    ChunkList chunks(fileID);

    while (!file.eof()) {
        buf = new char [CHUNK_SIZE];
        if (file.bad() || file.fail())
            throw ios_base::failure("Error reading file \"" + inFile + "\".");
        file.read(buf, CHUNK_SIZE);
        chunkCnt++;
        size_t bytesRead = file.gcount();
        string hash = ChunkHashSha256(buf, bytesRead);
        Chunk c = Chunk(hash, bytesRead);
        size_t storedSize = storageProvider->StoreChunk(c, buf);
        c.SetSize(storedSize);
        chunks.AddChunk(c);
        delete [] buf;
    }
    config->SaveFileChunks(chunks, snapshotId);
}

// https://stackoverflow.com/a/10632725/2465760
std::string FileChunker::ChunkHashSha256 (const char* data, size_t size) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256Ctx;
    SHA256_Init(& sha256Ctx);
    SHA256_Update(& sha256Ctx, data, size);
    SHA256_Final(digest, & sha256Ctx);
    ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        oss << hex << setw(2) << setfill('0') << static_cast<int>(digest[i]);
    return oss.str();
}

FileChunker::~FileChunker () {
    delete [] buf;
}

void FileChunker::RestoreFileFromChunks (string source, string destination, ChunkList chunks,
                                             string filePath, bool compressed) {
    ChunkListIterator it(& chunks);
    // string destinationFile = NormalizeDirectoryPath(destination) + filePath;
    if (fs::exists(filePath))
        fs::remove(filePath);
    else
        fs::create_directories(FilesystemUtils::GetDirectoryOfFilePath(filePath));

    unique_ptr<ChunkStorageProvider> storageProvider;

    if (compressed)
        storageProvider = make_unique<CompressedFilesystemChunkStorageProvider>
                (CompressedFilesystemChunkStorageProvider(source));
    else
        storageProvider = make_unique<FilesystemChunkStorageProvider>
                (FilesystemChunkStorageProvider(source));

    fstream file (filePath, ios::out | ios::binary);
    if (!file.is_open())
        throw runtime_error("Cannot open file " + filePath + ".");

    while (!it.End()) {
        Chunk current = it.Current();
        char* data = storageProvider->RetrieveChunk(current);
        file.write(data, current.GetSize());
        delete [] data;
        if (!file.good())
            throw runtime_error("Cannot write to file " + filePath + ".");
        it++;
    }
}