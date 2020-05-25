#ifndef BACKUP_FILE_H
#define BACKUP_FILE_H
#include "FilesystemEntity.h"

/**
 * Represents a file in a filesystem tree.
 */

class File : public FilesystemEntity {
    size_t m_Size = 0;
    size_t m_Ctime = 0;
    size_t m_Mtime = 0;
public:
    File(std::string path, size_t mtime, size_t size);
    ~File() override = default;
    size_t Size ();
    size_t Mtime ();
    bool IsFileModified ();

};


#endif //BACKUP_FILE_H
