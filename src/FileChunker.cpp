#include <filesystem>
#include <openssl/sha.h>
#include <fstream>
#include <ios>
#include <iostream>
#include "FileChunker.h"

using namespace std;

void FileChunker::GenerateFileChunks (std::string path) {
    fstream file = fstream(path, ios::in | ios::binary);
    char buf[CHUNK_SIZE];
    size_t chunks = 0;
    cout << path << ": ";
    while (!file.eof()) {
        if (file.bad() || file.fail())
            throw ios_base::failure("Error reading file " + path + ".");
        file.read(buf, CHUNK_SIZE);
        chunks++;
        size_t bytesRead = file.gcount();
        cout << chunks << " " << bytesRead << "B / hash: " << chunkHashSha256(buf, bytesRead) << " ";
        // todo store index in config (maybe create new class, but maybe simply call config function)
        // todo call backupstorageprovider to save actual data (buf)
    }
    cout << endl;
}

// https://stackoverflow.com/a/10632725/2465760
std::string FileChunker::chunkHashSha256 (char* data, size_t size) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256Ctx;
    SHA256_Init(& sha256Ctx);
    SHA256_Update(& sha256Ctx, data, size);
    SHA256_Final(digest, & sha256Ctx);
    ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        oss << hex << static_cast<int>(digest[i]);
    return oss.str();
}
