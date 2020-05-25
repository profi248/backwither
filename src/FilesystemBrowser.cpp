#include <filesystem>
#include <exception>
#include <memory>
#include "FilesystemBrowser.h"
#include "Directory.h"
#include "File.h"

namespace fs = std::filesystem;

Directory FilesystemBrowser::BrowseFolderRecursive (std::string path_str) {
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

        if (item.is_regular_file())
            root.AddFilesystemEntity(std::make_shared<FilesystemEntity>(File(item.path(), item.file_size())));

    }

    return root;
}
