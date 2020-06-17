#ifndef BACKUP_FILESYSTEMBROWSER_H
#define BACKUP_FILESYSTEMBROWSER_H
#include "Directory.h"
#include "BackupJob.h"
#include "ConfigProvider.h"
#include <string>

/**
 * Tools for accessing and browsing filesystem.
 */
class FilesystemUtils {
public:
    /**
     * Follow folder symlinks?
     */
    static const bool ENABLE_FOLDER_SYMLINKS = true;

    /**
     * Builds an index of a path and all subfolders.
     * @param path_str Absolute path to scan.
     * @return Directory structure.
     */

    static Directory BrowseFolderRecursive (std::string path_str);

    /**
     * Check if directory path is valid and writable, and create the directory if it doesn't exist.
     * @param destination Absolute path to directory.
     */
    static void VerifyOrCreateDestinationDirectory (std::string destination);

    /**
     * Check if directory is valid, readable and not empty.
     * @param source Absolute path to directory.
     */
    static void VerifySourceDirectory (std::string source);

    /**
     * Check if directory is empty.
     * @param path Absolute path to directory.
     * @return True if empty, false otherwise.
     */
    static bool IsDirectoryEmpty (std::string path);

    /**
     * Returns directory path with a slash at the end.
     * @param path Input path.
     * @return Normalized path.
     */
    static std::string NormalizeDirectoryPath (std::string path);

    /**
     * Finds parent directory of specified file (relative to backup root).
     * @param path Path to file.
     * @return Parent directory of specified file.
     */
    static std::string GetDirectoryOfFilePath (std::string path);

    /**
     * Converts a path to absolute (with resolving .. etc.).
     * @param path Path to convert.
     * @param create Whether to create directory if it doesn't esist already.
     * @return Canonical path.
     */
    static std::string AbsolutePath (std::string path, bool create = false);

    /**
     * Checks if specified paths resolve to identical path.
     * @param a First path.
     * @param b Second path.
     * @return True if equal, false otherwise.
     */
    static bool ArePathsEqual (std::string a, std::string b);
};


#endif //BACKUP_FILESYSTEMBROWSER_H
