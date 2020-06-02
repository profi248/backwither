#ifndef BACKUP_FILESYSTEMBROWSER_H
#define BACKUP_FILESYSTEMBROWSER_H
#include "Directory.h"
#include "BackupJob.h"
#include "ConfigProvider.h"
#include <string>

/**
 * Class for browsing filesystem.
 */
class FilesystemUtils {
public:
    /**
     * Builds a recursive tree of a filesystem path.
     * @param path_str Path to scan.
     * @return Directory structure.
     */

    static Directory   BrowseFolderRecursive (std::string path_str);
    static void        VerifyOrCreateDestinationDirectory (std::string destination);
    static void        VerifySourceDirectory (std::string source);
    static bool        IsDirectoryEmpty (std::string path);
    static std::string NormalizeDirectoryPath (std::string path);
    static std::string GetDirectoryOfFilePath (std::string path);
    static void
    RestoreFileFromChunks (std::string source, std::string destination, ChunkList chunks,
                           std::string filePath, bool compressed);
};


#endif //BACKUP_FILESYSTEMBROWSER_H
