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
    /**
     * Represents type of entity (file/folder/link).
     */
    enum FilesystemEntityType { FSENTITY, FILE, FOLDER, LINK };

    virtual ~FilesystemEntity () = default;

    /**
     * Get type of filesystem entity (file/folder/link).
     * @return Type of filesystem entity.
     */
    virtual FilesystemEntityType Type () {
        return FSENTITY;
    }

    /**
     * Get path of filesystem entity.
     * @return Path of filesystem entity.
     */
    virtual std::string Path () {
        return m_Path;
    }

};


#endif //BACKUP_FILESYSTEMENTITY_H
