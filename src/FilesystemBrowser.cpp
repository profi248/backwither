#include <filesystem>
#include <iostream>
#include <iomanip>
#include <exception>
#include "FilesystemBrowser.h"

namespace fs = std::filesystem;

Directory FilesystemBrowser::BrowseFolderRecursive (std::string path_str) {
    auto path = fs::path(path_str);
    if (!fs::exists(path))
        throw std::invalid_argument("Path does not exist.");

    auto de = fs::directory_entry(path);
    for (auto & item : fs::recursive_directory_iterator(path))
        std::cout << item.path() << '\n';

    return Directory();
}
