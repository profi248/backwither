#include <cstdlib>
#include <stdexcept>
#include <filesystem>
#include <sqlite3.h>
#include <ctime>
#include <memory>
#include <iostream>
#include "SQLiteBackupIndexProvider.h"
#include "FilesystemEntity.h"
#include "File.h"
#include "ChunkListIterator.h"

SQLiteBackupIndexProvider::SQLiteBackupIndexProvider (BackupJob* job) :
    m_Job (job) {
        m_Path = job->GetDestination();
        if (!m_Path.empty())
            if (m_Path[m_Path.length()] != '/')
                m_Path += '/';
        m_DB   = openDB();
    }

bool SQLiteBackupIndexProvider::initConfig () {
    sqlite3* db;

    if (sqlite3_open(getDbPath().c_str(), & db) != SQLITE_OK)
        throw std::runtime_error("Cannot create database in " + getDbPath() + ".");

    char* SQLiteError;

    int createSuccess =
    sqlite3_exec(db,
    "pragma encoding     = 'UTF-8';"
        "pragma foreign_keys = 1;"
        "pragma journal_mode = WAL;"   // more optimized writes
        "pragma synchronous  = NORMAL;"

        "create table settings (key text unique, value);"

        "create table snapshots (snapshot_id integer primary key asc, creation integer);"

        "create table files (file_id integer primary key asc, path text unique, size integer,"
        "mtime integer, ctime integer, snapshot_id integer references snapshots (snapshot_id));"

        "create table chunks (hash text, size integer,"
        "file_id integer references files (file_id),"
        "snapshot_id integer references snapshots (snapshot_id),"
        "position integer);"

        "create index chunks_idx on chunks (hash, file_id, position);"
        "create index file_idx on files (snapshot_id, path);"

        "insert into settings (key, value) values ('version', 1)",

       nullptr, nullptr, & SQLiteError);

    if (createSuccess != SQLITE_OK) {
        std::string err(SQLiteError);
        sqlite3_free(SQLiteError);
        sqlite3_close(db);
        throw std::runtime_error("SQLite error when creating DB schema in " + getDbPath() + ": " + err + ".");
    }

    sqlite3_close(db);
    return true;
}

bool SQLiteBackupIndexProvider::configExists () {
    sqlite3* db;
    if (sqlite3_open(getDbPath().c_str(), & db) != SQLITE_OK)
        return false;

    sqlite3_stmt* checkTblStmt;
    sqlite3_prepare_v2(db,
        "select 1 from sqlite_master where type='table' and name='settings';",
    SQLITE_NULL_TERMINATED, & checkTblStmt, nullptr);

    if (sqlite3_step(checkTblStmt) != SQLITE_ROW || sqlite3_column_int(checkTblStmt, 0) != 1) {
        sqlite3_finalize(checkTblStmt);
        sqlite3_close(db);
        return false;
    }

    sqlite3_finalize(checkTblStmt);

    sqlite3_stmt* checkVersionStmt;
    sqlite3_prepare_v2(db,
            "select value from settings where key = 'version' limit 1;",
            SQLITE_NULL_TERMINATED, & checkVersionStmt, nullptr);

    int version;

    if (sqlite3_step(checkVersionStmt) == SQLITE_ROW)
        version = sqlite3_column_int(checkVersionStmt, 0); // first column
    else {
        sqlite3_finalize(checkVersionStmt);
        sqlite3_close(db);
        return false;
    }

    if (version != 1) {
        sqlite3_finalize(checkVersionStmt);
        sqlite3_close(db);
        throw std::runtime_error("Invalid config version " + std::to_string(version));
    }

    sqlite3_finalize(checkVersionStmt);
    sqlite3_close(db);
    return true;
}

std::string SQLiteBackupIndexProvider::getDbPath () const {
    return m_Path + std::string(INDEX_DATABASE_FILENAME);
}

int64_t SQLiteBackupIndexProvider::SaveSnapshotFileIndex (Directory & fld) {
    sqlite3_stmt* addSnapshotStmt;

    if (sqlite3_exec(m_DB, "begin transaction;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        std::string err = sqlite3_errmsg(m_DB);
        throw std::runtime_error("Database error initializing a snapshot: " + err + ".");
    }

    sqlite3_prepare_v2(m_DB,
       "insert into snapshots (creation) values (?);",
       SQLITE_NULL_TERMINATED, & addSnapshotStmt, nullptr);

    sqlite3_bind_int64(addSnapshotStmt, 1, std::time(nullptr));

    if (sqlite3_step(addSnapshotStmt) != SQLITE_DONE) {
        sqlite3_finalize(addSnapshotStmt);
        std::string err = sqlite3_errmsg(m_DB);
        sqlite3_exec(m_DB, "rollback;", nullptr, nullptr, nullptr);

        throw std::runtime_error("Database error adding a new snapshot: " + err + ".");
    }

    int64_t snapshotID = sqlite3_last_insert_rowid(m_DB);
    sqlite3_finalize(addSnapshotStmt);

    DirectoryIterator it (& fld);
    sqlite3_stmt* addFileStmt;
    sqlite3_stmt* fileIdLookbackStmt;

    sqlite3_prepare_v2(m_DB,
       "insert or ignore into files (path, size, mtime, snapshot_id) values (?, ?, ?, ?);",
       SQLITE_NULL_TERMINATED, & addFileStmt, nullptr);

    sqlite3_prepare_v2(m_DB,
       "select file_id from files where path = ?;",
       SQLITE_NULL_TERMINATED, & fileIdLookbackStmt, nullptr);


    while (!it.End()) {
        // SQLITE_TRANSIENT: SQLite needs to make a copy of the string
        sqlite3_bind_text(addFileStmt, 1, it.GetPath().c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);
        sqlite3_bind_int64(addFileStmt, 2, it.GetSize());
        sqlite3_bind_int64(addFileStmt, 3, it.GetMtime());
        sqlite3_bind_int64(addFileStmt, 4, snapshotID);


        if (sqlite3_step(addFileStmt) != SQLITE_DONE) {
            const char * err = sqlite3_errmsg(m_DB);
            sqlite3_finalize(addFileStmt);
            sqlite3_finalize(fileIdLookbackStmt);
            sqlite3_exec(m_DB, "rollback;", nullptr, nullptr, nullptr);
            throw std::runtime_error("Database error when adding a file: " + std::string(err) + ".");
        } else {
            sqlite3_bind_text(fileIdLookbackStmt, 1, it.GetPath().c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);
            if (sqlite3_step(fileIdLookbackStmt) != SQLITE_ROW) {
                sqlite3_finalize(addFileStmt);
                sqlite3_finalize(fileIdLookbackStmt);
                throw std::runtime_error("Database error when adding a file.");
            }
            int64_t newFileID = sqlite3_column_int64(fileIdLookbackStmt, 0);
            it.SetID(newFileID);
        }

        sqlite3_reset(addFileStmt);
        sqlite3_reset(fileIdLookbackStmt);
        it++;
    }

    sqlite3_finalize(addFileStmt);
    sqlite3_finalize(fileIdLookbackStmt);
    if (sqlite3_exec(m_DB, "commit;", nullptr, nullptr, nullptr) != SQLITE_OK)
        throw std::runtime_error("Database error when creating a snapshot.");

    return snapshotID;
}

Directory SQLiteBackupIndexProvider::LoadSnapshotFileIndex (int64_t snapshotID) {
    Directory dir("/");

    sqlite3_stmt* loadFilesStmt;

    if (snapshotID != -1) {
        if (snapshotID == 0) {
            snapshotID = getLastSnapshotId(m_Job);
            if (snapshotID < 0)
                throw std::runtime_error("Last snapshot for backup not found. Maybe backup hasn't been run yet.");

        }
        sqlite3_prepare_v2(m_DB,
           "select path, size, mtime, file_id from files where snapshot_id = ?;",
           SQLITE_NULL_TERMINATED, & loadFilesStmt, nullptr);

        sqlite3_bind_int64(loadFilesStmt, 1, snapshotID);
    } else { // all files from all snapshots for this backup job
        sqlite3_prepare_v2(m_DB,
           "select path, mtime, size, file_id from files;",
           SQLITE_NULL_TERMINATED, & loadFilesStmt, nullptr);
    }

    while (sqlite3_step(loadFilesStmt) == SQLITE_ROW) {
        // SQLite returns unsigned char * (https://stackoverflow.com/a/804131)

        auto file = std::make_shared<File> (File(
            std::string(reinterpret_cast<const char*>(sqlite3_column_text(loadFilesStmt, 0))), // path
            sqlite3_column_int64(loadFilesStmt, 1), // size
            sqlite3_column_int64(loadFilesStmt, 2),  // mtime
            sqlite3_column_int64(loadFilesStmt, 3)  // id
        ));

        dir.AddFilesystemEntity(file);
    }

    sqlite3_finalize(loadFilesStmt);
    return dir;
}

int64_t SQLiteBackupIndexProvider::getLastSnapshotId (const BackupJob* job) {
    int64_t snapshotID;
    sqlite3_stmt* getSnapshotStmt;
    sqlite3_prepare_v2(m_DB,
       "select snapshot_id from snapshots order by snapshot_id desc limit 1;",
       SQLITE_NULL_TERMINATED, & getSnapshotStmt, nullptr);
    if (sqlite3_step(getSnapshotStmt) == SQLITE_ROW)
        snapshotID = sqlite3_column_int64(getSnapshotStmt, 0);  // snapshot_id
    else
        snapshotID = -1;

    sqlite3_finalize(getSnapshotStmt);
    return snapshotID;
}

void SQLiteBackupIndexProvider::SaveFileChunks (ChunkList chunks, int64_t snapshotId) {
    ChunkListIterator it (& chunks);
    sqlite3_stmt* saveChunksStmt;

    if (sqlite3_exec(m_DB, "begin transaction;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Database error initializing saving chunks.");
    }

    sqlite3_prepare_v2(m_DB,
       "insert into chunks (hash, size, file_id, snapshot_id, position) values (?, ?, ?, ?, ?);",
       SQLITE_NULL_TERMINATED, & saveChunksStmt, nullptr);


    size_t pos = 1;
    while (!it.End()) {
        // SQLITE_TRANSIENT: SQLite needs to make a copy of the string
        sqlite3_bind_text(saveChunksStmt, 1, it.GetHash().c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);
        sqlite3_bind_int64(saveChunksStmt, 2, it.GetSize());
        sqlite3_bind_int64(saveChunksStmt, 3, chunks.GetFileID());
        sqlite3_bind_int64(saveChunksStmt, 4, snapshotId);
        sqlite3_bind_int64(saveChunksStmt, 5, pos);

        if (sqlite3_step(saveChunksStmt) != SQLITE_DONE) {
            const char * err = sqlite3_errmsg(m_DB);
            sqlite3_finalize(saveChunksStmt);
            sqlite3_exec(m_DB, "rollback;", nullptr, nullptr, nullptr);

            throw std::runtime_error("Database error when saving chunks: " + std::string(err) + ".");
        }

        sqlite3_reset(saveChunksStmt);
        it++;
        pos++;
    }

    sqlite3_finalize(saveChunksStmt);
    if (sqlite3_exec(m_DB, "commit;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        const char * err = sqlite3_errmsg(m_DB);
        throw std::runtime_error("Database error when saving chunks: " + std::string(err) + ".");
    }

}

ChunkList SQLiteBackupIndexProvider::RetrieveFileChunks (BackupJob* job, int64_t snapshotId, int64_t fileId) {
    ChunkList chunks(fileId);
    sqlite3_stmt* retrieveChunksStmt;

    sqlite3_prepare_v2(m_DB,
       "select hash, size from chunks where file_id = ? and snapshot_id = ? order by position;",
       SQLITE_NULL_TERMINATED, & retrieveChunksStmt, nullptr);

    if (snapshotId == 0) {
        snapshotId = getLastSnapshotId(job);
        if (snapshotId < 0)
            throw std::runtime_error("Last snapshot for backup not found. Maybe backup hasn't been run yet.");
    }

    sqlite3_bind_int64(retrieveChunksStmt, 1, fileId);
    sqlite3_bind_int64(retrieveChunksStmt, 2, snapshotId);

    while (sqlite3_step(retrieveChunksStmt) == SQLITE_ROW) {
        // SQLite returns unsigned char * (https://stackoverflow.com/a/804131)
        Chunk chunk (
            std::string(reinterpret_cast<const char*>(sqlite3_column_text(retrieveChunksStmt, 0))), // hash
            sqlite3_column_int64(retrieveChunksStmt, 1)  // size
        );

        chunks.AddChunk(chunk);
    }

    sqlite3_finalize(retrieveChunksStmt);
    return chunks;
}


sqlite3* SQLiteBackupIndexProvider::openDB () {
    if (!configExists())
        initConfig();

    sqlite3* db;

    if (sqlite3_open(getDbPath().c_str(), & db) != SQLITE_OK)
        throw std::runtime_error("Cannot open the database.");

    int result = sqlite3_exec(db,
        "pragma encoding     = 'UTF-8';"
            "pragma foreign_keys = 1;"
            "pragma journal_mode = WAL;"   // more optimized writes
            "pragma synchronous  = NORMAL", nullptr, nullptr, nullptr);
    if (result != SQLITE_OK)
        throw std::runtime_error("Cannot set database parameters.");

    return db;
}

SQLiteBackupIndexProvider::~SQLiteBackupIndexProvider () {
    sqlite3_close(m_DB);
}

