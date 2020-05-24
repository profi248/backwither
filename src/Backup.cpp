#include <filesystem>
#include <fstream>
#include "Backup.h"
#include "FilesystemBrowser.h"

using namespace std;
namespace fs = filesystem;
// todo maybe refactor -> BackubJob::DoBackup;
int Backup::DoBackup (BackupJob* job) {
    std::string source = job->GetSource();
    std::string destination = job->GetDestination();

    verifySourceDirectory(source);
    verifyOrCreateDestinationDirectory(destination);

    if (!job->GetIncremental())
        return 3; // fixme implement !incremental

    FilesystemBrowser::BrowseFolderRecursive(source);

    return 0;
}

void Backup::verifySourceDirectory (std::string source) {
    if (!fs::exists(source))
        throw runtime_error("Backup source path doesn't exist.");

    if (!fs::is_directory(source))
        throw runtime_error("Backup source path is not a directory.");

    if (isDirectoryEmpty(source))
        throw runtime_error("Backup source directory is empty.");
}

void Backup::verifyOrCreateDestinationDirectory (std::string destination) {
    if (fs::exists(destination)) {
        if (fs::is_directory(destination)) {
            if (!isDirectoryEmpty(destination)) {
                throw runtime_error("Backup destination path is not empty.");
            }
        } else {
            throw runtime_error("Backup destination path is not a directory.");
        }
    } else {
        if (!fs::create_directories(destination))
            throw runtime_error("Directory can't be created in backup destination path.");
    }

    auto tmpFileStream = std::ofstream(normalizeDirectoryPath(destination) + ".tmp");

    if (!tmpFileStream.is_open() || !(tmpFileStream << "a"))
        throw runtime_error("Backup destination path is not writable.");

    if (!fs::remove(normalizeDirectoryPath(destination) + ".tmp"))
        throw runtime_error("Backup destination path is not writable.");
}


std::string Backup::normalizeDirectoryPath (std::string path) {
    if (path[path.length()] != '/')
        return path += '/';

    return path;
}

bool Backup::isDirectoryEmpty (std::string path) {
    return fs::begin(fs::directory_iterator(path)) == fs::end(fs::directory_iterator(path));
}

