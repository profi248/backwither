#ifndef BACKUP_SQLITEBACKUPINDEXPROVIDER_H
#define BACKUP_SQLITEBACKUPINDEXPROVIDER_H
#include <sqlite3.h>
#include "BackupIndexProvider.h"

/**
 * Implements config provider as a SQLite database.
 */
class SQLiteBackupIndexProvider : public BackupIndexProvider {
    const char* INDEX_DATABASE_FILENAME = "index.db";
    const int   SQLITE_NULL_TERMINATED = -1;

    BackupJob*  m_Job;
    std::string m_Path;
    sqlite3*    m_DB;

public:
    SQLiteBackupIndexProvider (BackupJob* job);
    ~SQLiteBackupIndexProvider () override;

    int64_t       SaveSnapshotFileIndex (Directory & fld) override;
    Directory     LoadSnapshotFileIndex (int64_t snapshotID) override;
    void          SaveFileChunks (ChunkList chunks, int64_t snapshotId) override;
    ChunkList     RetrieveFileChunks (BackupJob* job, int64_t snapshotId, int64_t fileId) override;
    SnapshotList* LoadSnapshotList () override;

private:
    std::string getDbPath () const;
    bool        configExists ();
    bool        initConfig ();
    sqlite3*    openDB ();
    int64_t     getLastSnapshotId (const BackupJob* job);
};

#endif //BACKUP_SQLITEBACKUPINDEXPROVIDER_H
