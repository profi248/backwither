#ifndef BACKUP_FILE_H
#define BACKUP_FILE_H
#include "FilesystemEntity.h"

/**
 * Represents a file in a filesystem tree.
 */

class File : public FilesystemEntity {
    ~File() override = default;
public:
    bool IsFileModified ();
};


#endif //BACKUP_FILE_H
