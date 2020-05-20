#ifndef BACKUP_FILESYSTEMBROWSER_H
#define BACKUP_FILESYSTEMBROWSER_H
#include "Directory.h"
#include <string>

/**
 * Class for browsing filesystem.
 */
class FilesystemBrowser {
public:
    /**
     * Builds a recursive tree of a filesystem path.
     * @param path_str Path to scan.
     * @return Directory structure.
     */

    static Directory* BrowseFolderRecursive (std::string path_str);
};


#endif //BACKUP_FILESYSTEMBROWSER_H
