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
    // list of files and subfolders
    // todo use custom comparator
    std::set<std::shared_ptr<FilesystemEntity>> m_Contents;

public:
    Directory(std::string path);
    ~Directory() override;
    void AddFilesystemEntity(std::shared_ptr<FilesystemEntity>);
    Directory operator - (Directory const & dir) const;
    Directory & operator = (Directory const & dir);

    friend class DirectoryIterator;

private:
    static bool compare(const std::shared_ptr<FilesystemEntity> & a,
                        const std::shared_ptr<FilesystemEntity> & b);
};

#endif //BACKUP_FILE_H
