#include <algorithm>
#include <fstream>
#include <filesystem>
#include <memory>
#include <chrono>
#include <iostream>
#include "FilesystemUtils.h"
#include "Directory.h"
#include "File.h"
#include "ChunkListIterator.h"
#include "FilesystemChunkStorageProvider.h"
#include "CompressedFilesystemChunkStorageProvider.h"

using namespace std;
namespace fs = filesystem;

Directory FilesystemUtils::BrowseFolderRecursive (string path_str) {
    auto path = fs::path(path_str);
    auto de = fs::directory_entry(path);

    Directory root("/");

    for (auto & item : fs::recursive_directory_iterator(path)) {
        // FilesystemEntity* e;

        // todo MAYBE remove directory altogether and make backup folder structure flat - put all files in a vector
        // file will have path in name, directory will be repurposed to just root directory, as it's used here
        // this will allow simple access to files and will be probably much easier overall
        /*
        if (item.is_directory()) {
            e = new Directory(item.path());
        } else if (item.is_regular_file()) {
            e = new File(item.path());
        } else if (item.is_symlink()) {
            // todo make symlinks work
            // e = new File(item.path());
        }
        */

        if (item.is_regular_file()) {
            fs::path filePath = item.path();
            root.AddFilesystemEntity(
                    make_shared<File>(File(filePath.lexically_proximate(path_str),
                            // time_point -> duration -> seconds
                            // https://gist.github.com/kantoniak/d58103623d0d7a7748fbc2040810428f
                            // fixme broken !!!
                            // fuck this
                           chrono::duration_cast<chrono::seconds>(
                                   item.last_write_time().time_since_epoch()).count(),
                           item.file_size(), -1))
            );
        }


    }

    return root;
}

void FilesystemUtils::VerifySourceDirectory (string source) {
    if (!fs::exists(source))
        throw runtime_error("Backup source path doesn't exist.");

    if (!fs::is_directory(source))
        throw runtime_error("Backup source path is not a directory.");

    if (IsDirectoryEmpty(source))
        throw runtime_error("Backup source directory is empty.");
}

void FilesystemUtils::VerifyOrCreateDestinationDirectory (string destination) {
    if (fs::exists(destination)) {
        if (!fs::is_directory(destination))
            throw runtime_error("Backup destination path is not a directory.");

    } else {
        if (!fs::create_directories(destination))
            throw runtime_error("Directory can't be created in backup destination path.");
    }

    auto tmpFileStream = ofstream(NormalizeDirectoryPath(destination) + ".tmp");

    if (!tmpFileStream.is_open() || !(tmpFileStream << "a"))
        throw runtime_error("Backup destination path is not writable.");

    if (!fs::remove(NormalizeDirectoryPath(destination) + ".tmp"))
        throw runtime_error("Backup destination path is not writable.");
}

string FilesystemUtils::NormalizeDirectoryPath (string path) {
    // todo remake this https://en.cppreference.com/w/cpp/filesystem/path
    if (path[path.length()] != '/')
        return path += '/';

    return path;
}

string FilesystemUtils::GetDirectoryOfFilePath (string path) {
    int lastSeparator = -1;
    for (size_t i = 0; i < path.length(); i++) {
        if (path[i] == '/')
            lastSeparator = i;
    }

    if (lastSeparator < 0)
        return path;

    return path.substr(0, lastSeparator + 1);
}

bool FilesystemUtils::IsDirectoryEmpty (string path) {
    return fs::begin(fs::directory_iterator(path)) == fs::end(fs::directory_iterator(path));
}

void FilesystemUtils::RestoreFileFromChunks (std::string source, std::string destination, ChunkList chunks,
                                             std::string filePath) {
    ChunkListIterator it(& chunks);
    // string destinationFile = NormalizeDirectoryPath(destination) + filePath;
    if (fs::exists(filePath))
        fs::remove(filePath);
    else
        fs::create_directories(GetDirectoryOfFilePath(filePath));

    fstream file (filePath, ios::out | ios::binary);
    auto storageProvider = CompressedFilesystemChunkStorageProvider(source);
    if (!file.is_open())
        throw runtime_error("Cannot open file " + filePath + ".");

    while (!it.End()) {
        Chunk current = it.Current();
        char* data = storageProvider.RetrieveChunk(current);
        file.write(data, current.GetSize());
        delete [] data;
        if (!file.good())
            throw runtime_error("Cannot write to file " + filePath + ".");
        it++;
    }
}

