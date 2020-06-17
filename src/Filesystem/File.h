#ifndef BACKUP_FILE_H
#define BACKUP_FILE_H
#include "FilesystemEntity.h"

/**
 * Represents a file in a filesystem tree.
 */

class File : public FilesystemEntity {
    size_t     m_Size  =  0;
    long long  m_Mtime =  0;
    int64_t    m_ID    = -1;
public:
    /**
     * Construct a file.
     * @param path Path relative to backup location.
     * @param mtime Last modification time from filesystem.
     * @param size Size in bytes.
     * @param id ID of file in DB, if known, else -1.
     */
    File (std::string path, long long mtime, size_t size, int64_t id = -1);
    ~File() override = default;
    /**
     * Get size of file in bytes.
     * @return Size.
     */
    size_t GetSize () const;
    /**
     * Get last modification time of file.
     * @return Unix timestamp of last file modification.
     */
    long long GetMtime () const;
    /**
     * Get ID of file from DB.
     * @return ID of file, -1 if unknown.
     */
    int64_t GetID () const;
    /**
     * Set ID of file.
     * @param id ID in DB.
     */
    void SetID (int64_t id);
};


#endif //BACKUP_FILE_H
