#ifndef BACKWITHER_BACKUPINDEXPROVIDER_H
#define BACKWITHER_BACKUPINDEXPROVIDER_H
#include <string>
#include "Directory.h"
#include "BackupJob.h"
#include "SnapshotList.h"
#include "ChunkList.h"
#include "SnapshotList.h"

class BackupIndexProvider {
public:
    virtual ~BackupIndexProvider () = default;

    virtual int64_t       SaveSnapshotFileIndex (Directory & fld) = 0;
    virtual Directory     LoadSnapshotFileIndex (int64_t snapshotID) = 0;
    virtual void          SaveFileChunks (ChunkList chunks, int64_t snapshotId) = 0;
    virtual ChunkList     RetrieveFileChunks (BackupJob* job, int64_t snapshotId, int64_t fileId) = 0;
    virtual SnapshotList* LoadSnapshotList () = 0;
    virtual Snapshot      GetSnapshot (int64_t id) = 0;
    virtual long long     LastSuccessfulCompletion () = 0;
    virtual void          FinalizeBackup (BackupJob* job) = 0;
};


#endif //BACKWITHER_BACKUPINDEXPROVIDER_H
