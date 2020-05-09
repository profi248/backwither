#ifndef BACKUP_FILESYSTEMBROWSER_H
#define BACKUP_FILESYSTEMBROWSER_H
#include "Directory.h"
#include <string>

/**
 * Class for browsing filesystem.
 */
class FilesystemBrowser {
    /**
     * Builds a recursive tree of a filesystem path.
     * @param path Path to scan.
     * @return Directory structure.
     */

    Directory BrowseFolderRecursive (std::string path);
};


#endif //BACKUP_FILESYSTEMBROWSER_H
