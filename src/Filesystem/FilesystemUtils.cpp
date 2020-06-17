#include <algorithm>
#include <fstream>
#include <filesystem>
#include <memory>
#include <iostream>
#include "FilesystemUtils.h"
#include "File.h"

using namespace std;
namespace fs = filesystem;

Directory FilesystemUtils::BrowseFolderRecursive (string path_str) {
    auto path = fs::path(path_str);
    auto de = fs::directory_entry(path);

    Directory root;
    fs::directory_options directoryOptions = {};

    if (ENABLE_FOLDER_SYMLINKS)
        directoryOptions |= fs::directory_options::follow_directory_symlink;

    for (auto & item : fs::recursive_directory_iterator(path, directoryOptions)) {
        if (item.is_regular_file()) {
            fs::path filePath = item.path();
            root.AddFilesystemEntity(
                make_shared<File>(File(filePath.lexically_proximate(path_str),
                    static_cast<long long>(TimeUtils::toTimeT(item.last_write_time())),
                    item.file_size()))
            );
        }
    }

    return root;
}

void FilesystemUtils::VerifySourceDirectory (string source) {
    if (!fs::exists(source))
        throw runtime_error("Directory \"" + source + "\" doesn't exist.");

    if (!fs::is_directory(source))
        throw runtime_error("Path \"" + source + "\" is not a directory.");

    if (IsDirectoryEmpty(source))
        throw runtime_error("Directory \"" + source + "\" is empty.");
}

void FilesystemUtils::VerifyOrCreateDestinationDirectory (string destination) {
    if (fs::exists(destination)) {
        if (!fs::is_directory(destination))
            throw runtime_error("Path \"" + destination + "\" is not a directory.");
    } else {
        if (!fs::create_directories(destination))
            throw runtime_error("Directory can't be created in \"" + destination + "\".");
    }

    auto tmpFileStream = ofstream(NormalizeDirectoryPath(destination) + ".tmp");

    if (!tmpFileStream.is_open() || !(tmpFileStream << "a"))
        throw runtime_error("Directory \"" + destination + "\" not writable.");

    if (!fs::remove(NormalizeDirectoryPath(destination) + ".tmp"))
        throw runtime_error("Directory \"" + destination + "\" not writable.");
}

string FilesystemUtils::NormalizeDirectoryPath (string path) {
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
    if (!fs::exists(path))
        throw runtime_error("Directory \"" + path + "\" does not exist.");
    return fs::begin(fs::directory_iterator(path)) == fs::end(fs::directory_iterator(path));
}

string FilesystemUtils::AbsolutePath (std::string path, bool create) {
    if (fs::exists(path))
        return fs::canonical(path);
    else {
        if (create) {
            fs::create_directories(path);
            return fs::canonical(path);
        } else
            throw runtime_error("Backup path \"" + path + "\" does not exist.");
    }
}

bool FilesystemUtils::ArePathsEqual (std::string a, std::string b) {
    return fs::equivalent(a, b);
}

