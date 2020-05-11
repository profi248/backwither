#ifndef BACKUP_LINK_H
#define BACKUP_LINK_H
#include "FilesystemEntity.h"

/**
 * Represents a link in a filesystem tree.
 */

class Link : public FilesystemEntity {
    FilesystemEntity* m_LinkedFilesystemEntity;
    ~Link() override = default;
public:
    /**
     * Return linked filesystem entity.
     * @return Linked filesystem entity.
     */
    FilesystemEntity* GetLinkedFilesystemEntity ();
};


#endif //BACKUP_LINK_H
