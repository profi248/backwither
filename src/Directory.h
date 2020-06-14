#ifndef BACKUP_DIRECTORY_H
#define BACKUP_DIRECTORY_H
#include <set>
#include <string>
#include <memory>
#include "FilesystemEntity.h"
#include "DirectoryIterator.h"

/**
 * Represents a folder in a filesystem tree.
 */
class DirectoryIterator;

class Directory : public FilesystemEntity {
    /// list of files
    std::set<std::shared_ptr<FilesystemEntity>, FilesystemEntity::Compare> m_Contents;

public:
    Directory(std::string path);
    ~Directory() override;
    void        AddFilesystemEntity(std::shared_ptr<FilesystemEntity>);
    size_t      EntityCount();
    Directory   operator - (Directory const & dir) const;
    Directory   operator + (Directory const & dir) const;

    friend class DirectoryIterator;
};

#endif //BACKUP_FILE_H
