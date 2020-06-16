#ifndef BACKWITHER_BACKUPINDEXPROVIDER_H
#define BACKWITHER_BACKUPINDEXPROVIDER_H
#include <string>
#include "Directory.h"
#include "BackupJob.h"
#include "SnapshotList.h"
#include "ChunkList.h"
#include "SnapshotList.h"

/**
 * Class providing interface for saving and retrieving info about files and chunks in a backup.
 */
class BackupIndexProvider {
public:
    virtual ~BackupIndexProvider () = default;

    /**
     * Create new snapshot with all files in supplied folder (data or paths are not actually duplicated).
     * @param fld Folder to save.
     * @return ID of the newly created snapshot.
     */
    virtual int64_t SaveSnapshotFileIndex (Directory & fld) = 0;

    /**
     * Load directory index by snapshot ID.
     * @param snapshotID Snapshot ID to retrieve.
     * @return Directory with files from snapshot.
     */
    virtual Directory LoadSnapshotFileIndex (int64_t snapshotID) = 0;

    /**
     * Save a list of chunks in a file to a snapshot (how is this version of the file assembled).
     * @param chunks List of chunks to save.
     * @param snapshotId Snapshot ID to save it to.
     */
    virtual void SaveFileChunks (ChunkList chunks, int64_t snapshotId) = 0;

    /**
     * Load list of chunks of a specific file from a specific snapshot (0 for latest).
     * @param snapshotId Which snapshot ID to retrieve from.
     * @param fileId Which file ID to retrieve.
     * @return ChunkList of chunks in this file for a specific snapshot.
     */
    virtual ChunkList RetrieveFileChunks (int64_t snapshotId, int64_t fileId) = 0;

    /**
     * Get all snapshots of a backup.
     * @return SnapshotList pointer.
     */
    virtual SnapshotList* LoadSnapshotList () = 0;

    /**
     * Get info about specific snapshot.
     * @param id Snapshot ID.
     * @return Snapshot object.
     */
    virtual Snapshot GetSnapshot (int64_t id) = 0;

    /**
     * Get last successful run of the backup (completion of lastest snapshot).
     * @return Unix timestamp of last backup completion.
     */
    virtual long long LastSuccessfulCompletion () = 0;

    /**
     * Check if a specific file exists in a specific snapshot.
     * @param snapshotId ID of a snapshot to check.
     * @param filePath Relative path to file.
     * @return True if file exists.
     */
    virtual bool DoesFileExistInSnapshot (int64_t snapshotId, std::string filePath) = 0;

    /**
     * Checks from storage if backup is compressed (to verify if it matches setting in backup list).
     * @return True if compressed.
     */
    virtual bool GetCompressed () = 0;

    /**
     * Save snapshot completion time and possibly do some cleanup.
     */
    virtual void FinalizeBackup () = 0;
};


#endif //BACKWITHER_BACKUPINDEXPROVIDER_H
