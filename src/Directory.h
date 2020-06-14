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
    /// List of files
    std::set<std::shared_ptr<FilesystemEntity>, FilesystemEntity::Compare> m_Contents;

public:
    Directory () = default;
    ~Directory() override;
    /**
     * Add a fileystem entity to the directory
     * @param e The filesystem entity to add.
     */
    void AddFilesystemEntity(std::shared_ptr<FilesystemEntity> e);
    /**
     * Count entities in directory.
     * @return Number of entities in directory.
     */
    size_t EntityCount();
    /**
     * Compute difference between contents of two directories.
     * @param dir Second directory.
     * @return Difference of directories.
     */
    Directory   operator - (Directory const & dir) const;
    /**
     * Combine contents of two directories.
     * @param dir Second directory.
     * @return Combined directory.
     */
    Directory   operator + (Directory const & dir) const;

    friend class DirectoryIterator;
};

#endif //BACKUP_FILE_H
