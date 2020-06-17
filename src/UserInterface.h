#ifndef BACKUP_USERINTERFACE_H
#define BACKUP_USERINTERFACE_H
#include "BackupJob.h"

/**
 * Provides user interface for backup management.
 */

class UserInterface {
public:
    virtual ~UserInterface () = default;
    /**
     * Start user interface (open window, parse arguments...).
     *
     * @param argc Argument count.
     * @param argv Arguments.
     * @return Return code.
     */
    virtual int StartInterface (int argc, char** argv) = 0;

    /**
     * Update user interface while backup is ongoing.
     *
     * @param current Current number of files.
     * @param expected Expected number of files.
     * @param status Status string.
     * @param fileSize Size of current file.
     */
    virtual void UpdateProgress (size_t current, size_t expected, std::string status, size_t fileSize) = 0;

    /**
     * Convert bytes to user comprehensible size.
     *
     * @param bytes Input size.
     * @return String with rounded size in reasonable units.
     */
    static std::string HumanFileSize (size_t bytes);

    /**
     * Get program version.
     * @return version string
     */
    static std::string GetVersion ();

};


#endif //BACKUP_USERINTERFACE_H
