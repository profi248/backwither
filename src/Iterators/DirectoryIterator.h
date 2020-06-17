#ifndef BACKWITHER_DIRECTORYITERATOR_H
#define BACKWITHER_DIRECTORYITERATOR_H
#include <set>
#include "../Filesystem/Directory.h"
#include "SimpleIterator.h"

/**
 * Allow iterating over items in directory.
 */

class Directory;

class DirectoryIterator : public SimpleIterator {
    Directory*                                                                       m_Directory;
    std::set<std::shared_ptr<FilesystemEntity>, FilesystemEntity::Compare>::iterator m_DirectoryIt;
public:
    explicit DirectoryIterator (Directory * dir);
    ~DirectoryIterator () override = default;

    void                               Next () override;
    void                               Rewind () override;

    /**
     * Get current FilesystemEntity.
     * @return std::shared_ptr<FilesystemEntity>.
     */
    std::shared_ptr<FilesystemEntity>  Current () const;

    /**
     * Get current path.
     * @return Path.
     */
    std::string                        GetPath () const;

    /**
     * Get current modification time, if file (0 otherwise).
     * @return Unix timestamp of modification time.
     */
    int64_t                            GetMtime () const;

    /**
     * Get current size, if file (0 otherwise).
     * @return Current size in bytes.
     */
    int64_t                            GetSize () const;

    /**
     * Get current ID, if file (0 otherwise) and ID is known (-1 otherwise).
     * @return File ID.
     */
    int64_t                            GetID () const;

    /**
     * Get current ID, if file.
     * @param id File ID.
     */
    void                               SetID (int64_t id);
    bool                               End () const override;
    bool                               Empty () const override;

    std::vector<std::string> TableHeader () const override;
    std::vector<std::string> TableRow () const override;

    void operator ++ (int) override;
};


#endif //BACKWITHER_DIRECTORYITERATOR_H
