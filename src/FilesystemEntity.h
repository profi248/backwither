#ifndef BACKUP_FILESYSTEMENTITY_H
#define BACKUP_FILESYSTEMENTITY_H

#include <string>

/**
 * Base class for filesystem entities (file/folder/link).
 */

class FilesystemEntity {
    std::string m_Path;
    std::string m_Name;
public:
    virtual ~FilesystemEntity () = default;

    /**
     * Get path of filesystem entity.
     * @return Path of filesystem entity.
     */
    virtual std::string Path () {
        return m_Path;
    }

};


#endif //BACKUP_FILESYSTEMENTITY_H
