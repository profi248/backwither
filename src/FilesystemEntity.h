#ifndef BACKUP_FILESYSTEMENTITY_H
#define BACKUP_FILESYSTEMENTITY_H

#include <string>
#include <memory>

/**
 * Base class for filesystem entities (file/folder/link).
 */

class FilesystemEntity {
protected:
    std::string m_Path;
public:
    virtual ~FilesystemEntity () = default;

    /**
     * Get path of filesystem entity.
     * @return Path of filesystem entity.
     */
    virtual std::string Path () {
        return m_Path;
    }

    /**
     * Compare FilesystemEntities by path.
     * @param a First value.
     * @param b Second value.
     * @return bool (a < b)
     */

    static bool Cmp (const std::shared_ptr<FilesystemEntity> & a,
                     const std::shared_ptr<FilesystemEntity> & b) {
        return a->Path() < b->Path();
    }

    struct Compare {
        /**
         * Functor for set ordering by path.
         * @return bool (a < b)
         */
        bool operator () (const std::shared_ptr<FilesystemEntity> & a,
                          const std::shared_ptr<FilesystemEntity> & b) const {
            return Cmp(a, b);
        }
    };
};

#endif //BACKUP_FILESYSTEMENTITY_H
