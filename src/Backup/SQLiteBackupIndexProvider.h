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

    /**
     * Construct index provider for a specific job.
     * @param job The backup job.
     */
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
    bool          GetCompressed () override;
    int64_t       GetLastSnapshotId () override;
    void          FinalizeBackup () override;

private:
    // helper for preparing a single query
    int         prepareOne(const char* sql, sqlite3_stmt** stmt);
    // determine DB path
    std::string getDbPath () const;
    // check if DB already exists
    bool        configExists ();
    // create a new DB
    bool        initConfig ();
    // open existing DB
    sqlite3*    openDB ();

};

#endif //BACKUP_SQLITEBACKUPINDEXPROVIDER_H
