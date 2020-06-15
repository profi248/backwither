#ifndef BACKUP_SQLITEBACKUPINDEXPROVIDER_H
#define BACKUP_SQLITEBACKUPINDEXPROVIDER_H
#include <sqlite3.h>
#include "BackupIndexProvider.h"

/**
 * Implements backup index provider (list of files in snapshots and chunks that make up data) as a SQLite database.
 */
class SQLiteBackupIndexProvider : public BackupIndexProvider {
    const int   SQLITE_NULL_TERMINATED = -1;

    BackupJob*  m_Job;
    std::string m_Path;
    sqlite3*    m_DB;

public:
    const char* INDEX_DATABASE_FILENAME = "index.db";

    SQLiteBackupIndexProvider (BackupJob* job);
    ~SQLiteBackupIndexProvider () override;

    int64_t       SaveSnapshotFileIndex (Directory & fld) override;
    Directory     LoadSnapshotFileIndex (int64_t snapshotID) override;
    void          SaveFileChunks (ChunkList chunks, int64_t snapshotId) override;
    ChunkList     RetrieveFileChunks (int64_t snapshotId, int64_t fileId) override;
    SnapshotList* LoadSnapshotList () override;
    Snapshot      GetSnapshot (int64_t id) override;
    long long     LastSuccessfulCompletion () override;
    bool          DoesFileExistInSnapshot (int64_t snapshotId, std::string filePath) override;
    void          FinalizeBackup () override;

private:
    int         prepareOne(const char* sql, sqlite3_stmt** stmt);
    std::string getDbPath () const;
    bool        configExists ();
    bool        initConfig ();
    sqlite3*    openDB ();
    int64_t     getLastSnapshotId ();

};

#endif //BACKUP_SQLITEBACKUPINDEXPROVIDER_H
