#ifndef BACKUP_USERINTERFACE_H
#define BACKUP_USERINTERFACE_H
#include "BackupJob.h"

/**
 * Provides user interface for backup management.
 */

class UserInterface {
public:
    virtual ~UserInterface () = default;
    virtual int  StartInterface (int argc, char** argv) = 0;
    virtual void UpdateProgress (size_t current, size_t expected, std::string status, size_t fileSize) = 0;

    static std::string HumanFileSize (size_t bytes);
};


#endif //BACKUP_USERINTERFACE_H
