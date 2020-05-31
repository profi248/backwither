#ifndef BACKUP_FILE_H
#define BACKUP_FILE_H
#include "FilesystemEntity.h"

/**
 * Represents a file in a filesystem tree.
 */

class File : public FilesystemEntity {
    size_t  m_Size  =  0;
    size_t  m_Ctime =  0;
    size_t  m_Mtime =  0;
    int64_t m_ID    = -1;
public:
    File (std::string path, size_t mtime, size_t size, int64_t id = -1);
    ~File() override = default;
    size_t  GetSize ();
    size_t  GetMtime ();
    int64_t GetID ();
    void    SetID (int64_t id);
    bool IsFileModified ();

};


#endif //BACKUP_FILE_H
