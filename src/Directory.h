#ifndef BACKUP_DIRECTORY_H
#define BACKUP_DIRECTORY_H
#include <vector>
#include <string>
#include "FilesystemEntity.h"

/**
 * Represents a folder in a filesystem tree.
 */


class Directory : public FilesystemEntity {
    // list of files and subfolders
    std::vector<FilesystemEntity*> m_Contents;

public:
    ~Directory() override = default;

    FilesystemEntityType Type () override {
        return FilesystemEntity::FOLDER;
    }

};


#endif //BACKUP_FILE_H
