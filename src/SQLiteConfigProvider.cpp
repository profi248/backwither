#include <cstdlib>
#include <stdexcept>
#include <filesystem>
#include <sqlite3.h>
#include <ctime>
#include <memory>
#include "SQLiteConfigProvider.h"
#include "FilesystemEntity.h"
#include "File.h"

const char* CONFIG_FOLDER_FILENAME   = "backwither";
const char* CONFIG_DATABASE_FILENAME = "config.db";
const char* CONFIG_DEFAULT_PATH      = ".config";
const char* CONFIG_HOME_ENVVAR       = "HOME";
const char* CONFIG_DEFAULT_ENVVAR    = "XDG_CONFIG_HOME";

const int   SQLITE_NULL_TERMINATED = -1;

// todo maybe move SQLite object to member var

SQLiteConfigProvider::SQLiteConfigProvider (std::string path) {
    if (!path.empty())
        if (path[path.length()] != '/')
            path += '/';
    m_Path = path;
}

std::string SQLiteConfigProvider::getDefaultConfigPath () const {
    std::string path;
    // https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html
    if (std::getenv(CONFIG_DEFAULT_ENVVAR)) // return can be null
        path = std::getenv(CONFIG_DEFAULT_ENVVAR);
    if (path.empty()) {
        if (std::getenv(CONFIG_HOME_ENVVAR))
            path = std::getenv(CONFIG_HOME_ENVVAR);

        if (path.empty()) {
            throw std::runtime_error("Neither " + std::string(CONFIG_HOME_ENVVAR)
            + " or " + std::string(CONFIG_DEFAULT_ENVVAR) +
            " env variables are set. Cannot find default path for config.");
        } else {
            path += "/" + std::string(CONFIG_DEFAULT_PATH) + "/";
        }
    }

    path += std::string(CONFIG_FOLDER_FILENAME) + "/";
    std::filesystem::create_directories(path);

    return path;
}

bool SQLiteConfigProvider::initConfig () {
    sqlite3* db;

    std::string path = m_Path;
    if (path.empty())
        path = getDefaultConfigPath();

    if (!std::filesystem::exists(path))
        if (!std::filesystem::create_directory(path))
            throw std::runtime_error("Config directory " + path + " cannot be created.");

    if (sqlite3_open(getDbPath().c_str(), & db) != SQLITE_OK)
        throw std::runtime_error("Cannot create database in " + getDbPath() + ".");

    char* SQLiteError;

    int createSuccess =
    sqlite3_exec(db,
    "pragma foreign_keys = 1;"

        "create table settings (key text unique, value);"

        "create table backups (backup_id integer primary key asc, source text,"
        "destination text, name text unique, incremental integer);"

        "create table snapshots (snapshot_id integer primary key asc, creation integer,"
        "backup_id integer references backups (backup_id));"

        "create table files (file_id integer primary key asc, path text, size integer,"
        "mtime integer, ctime integer, snapshot_id integer references snapshots (snapshot_id),"
        "backup_id integer references backups (backup_id));"

        "create table chunkdata (chunk_id integer primary key asc, hash text, size integer);"

        "create table filechunks (chunk_id integer references chunkdata (chunk_id),"
        "file_id integer references files (file_id), position integer);"

        "create index chunkdata_idx on chunkdata (hash);"
        "create index filechunks_idx on filechunks (file_id, position);"
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

bool SQLiteConfigProvider::configExists () {
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

std::string SQLiteConfigProvider::getDbPath () const {
    if (m_Path.empty())
        return getDefaultConfigPath() + std::string(CONFIG_DATABASE_FILENAME);
    else
        return m_Path + std::string(CONFIG_DATABASE_FILENAME);
}


void SQLiteConfigProvider::AddBackupJob (BackupJob* job) {
     sqlite3* db = openDB();
     sqlite3_stmt* addJobStmt;

    sqlite3_prepare_v2(db,
       "insert into backups (source, destination, name, incremental) values (?, ?, ?, ?);",
       SQLITE_NULL_TERMINATED, & addJobStmt, nullptr);

    // SQLITE_TRANSIENT: SQLite needs to make a copy of the string
    sqlite3_bind_text(addJobStmt, 1, job->GetSource().c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);
    sqlite3_bind_text(addJobStmt, 2, job->GetDestination().c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);
    sqlite3_bind_text(addJobStmt, 3, job->GetName().c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);
    sqlite3_bind_int(addJobStmt, 4, static_cast<int>(job->GetIncremental()));

    if (sqlite3_step(addJobStmt) != SQLITE_DONE) {
        sqlite3_finalize(addJobStmt);
        sqlite3_close(db);

        throw std::runtime_error("Error when adding a new backup job. Name might already be taken.");

    }

    sqlite3_finalize(addJobStmt);
    sqlite3_close(db);
}


BackupJob* SQLiteConfigProvider::GetBackupJob (std::string name) {
    sqlite3* db = openDB();

    sqlite3_stmt* getBackupJobStmt;
    sqlite3_prepare_v2(db,
        "select backup_id, source, destination, incremental from backups where name = ?;",
        SQLITE_NULL_TERMINATED, & getBackupJobStmt, nullptr);

    // SQLITE_TRANSIENT: SQLite needs to make a copy of the string
    sqlite3_bind_text(getBackupJobStmt, 1, name.c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);

    if (sqlite3_step(getBackupJobStmt) == SQLITE_ROW) {
        int64_t id = sqlite3_column_int64(getBackupJobStmt, 0); // first column
        std::string source = reinterpret_cast<const char*>(sqlite3_column_text(getBackupJobStmt, 1));
        std::string destination = reinterpret_cast<const char*>(sqlite3_column_text(getBackupJobStmt, 2));
        bool incremental = static_cast<bool>(sqlite3_column_int(getBackupJobStmt, 3));

        sqlite3_finalize(getBackupJobStmt);
        sqlite3_close(db);

        return new BackupJob(source, destination, name, incremental, id);
    } else {
        sqlite3_finalize(getBackupJobStmt);
        sqlite3_close(db);
        return nullptr;
    }

}

BackupPlan* SQLiteConfigProvider::LoadBackupPlan () {
    BackupPlan* plan = new BackupPlan();
    sqlite3* db = openDB();

    sqlite3_stmt* loadPlanStmt;

    sqlite3_prepare_v2(db,
       "select source, destination, name, incremental from backups order by name;",
       SQLITE_NULL_TERMINATED, & loadPlanStmt, nullptr);

    while (sqlite3_step(loadPlanStmt) == SQLITE_ROW) {
        // SQLite returns unsigned char * (https://stackoverflow.com/a/804131)
        BackupJob* job = new BackupJob (
            std::string(reinterpret_cast<const char*>(sqlite3_column_text(loadPlanStmt, 0))), // source
            std::string(reinterpret_cast<const char*>(sqlite3_column_text(loadPlanStmt, 1))), // destination
            std::string(reinterpret_cast<const char*>(sqlite3_column_text(loadPlanStmt, 2))), // name
            static_cast<bool>(sqlite3_column_int(loadPlanStmt, 3))  // incremental
        );

        plan->AddBackup(job);
    }

    sqlite3_finalize(loadPlanStmt);
    sqlite3_close(db);
    return plan;
}

void SQLiteConfigProvider::SaveSnapshotFileIndex (Directory fld, BackupJob *job) {
    sqlite3* db = openDB();
    sqlite3_stmt* addSnapshotStmt;

    if (sqlite3_exec(db, "begin transaction;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        throw std::runtime_error("Database error initializing a snapshot.");
    }

    sqlite3_prepare_v2(db,
       "insert into snapshots (creation, backup_id) values (?, ?);",
       SQLITE_NULL_TERMINATED, & addSnapshotStmt, nullptr);

    sqlite3_bind_int64(addSnapshotStmt, 1, std::time(nullptr));
    sqlite3_bind_int64(addSnapshotStmt, 2, job->GetID());

    if (sqlite3_step(addSnapshotStmt) != SQLITE_DONE) {
        sqlite3_finalize(addSnapshotStmt);
        sqlite3_exec(db, "rollback;", nullptr, nullptr, nullptr);
        sqlite3_close(db);

        throw std::runtime_error("Database error adding a new snapshot.");
    }

    int64_t snapshotID = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(addSnapshotStmt);

    DirectoryIterator it (& fld);
    sqlite3_stmt* addFileStmt;

    sqlite3_prepare_v2(db,
           "insert into files (path, size, mtime, snapshot_id, backup_id) values (?, ?, ?, ?, ?);",
           SQLITE_NULL_TERMINATED, & addFileStmt, nullptr);


    while (!it.End()) {
        // SQLITE_TRANSIENT: SQLite needs to make a copy of the string
        sqlite3_bind_text(addFileStmt, 1, it.GetPath().c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);
        sqlite3_bind_int64(addFileStmt, 2, it.GetSize());
        sqlite3_bind_int64(addFileStmt, 3, it.GetMtime());
        sqlite3_bind_int64(addFileStmt, 4, snapshotID);
        sqlite3_bind_int64(addFileStmt, 5, job->GetID());

        if (sqlite3_step(addFileStmt) != SQLITE_DONE) {
            sqlite3_finalize(addFileStmt);
            sqlite3_exec(db, "rollback;", nullptr, nullptr, nullptr);
            sqlite3_close(db);

            throw std::runtime_error("Database error when adding a file.");
        }

        sqlite3_reset(addFileStmt);
        it++;
    }

    sqlite3_finalize(addFileStmt);
    if (sqlite3_exec(db, "commit;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        throw std::runtime_error("Database error when creating a snapshot.");
    }

    sqlite3_close(db);
}

Directory SQLiteConfigProvider::LoadSnapshotFileIndex (BackupJob* job, int64_t snapshotID) {
    Directory dir("/");
    sqlite3* db = openDB();

    sqlite3_stmt* loadFilesStmt;

    if (snapshotID != -1) {
        if (snapshotID == 0) {
            snapshotID = getLastSnapshotId(job);
            if (snapshotID < 0) {
                sqlite3_close(db);
                throw std::runtime_error("Last snapshot for backup not found. Maybe backup hasn't been run yet.");
            }
        }
        sqlite3_prepare_v2(db,
           "select path, size, mtime from files where snapshot_id = ?;",
           SQLITE_NULL_TERMINATED, & loadFilesStmt, nullptr);

        sqlite3_bind_int64(loadFilesStmt, 1, snapshotID);
    } else { // all files from all snapshots for this backup job
        sqlite3_prepare_v2(db,
           "select path, size, mtime from files where backup_id = ?;",
           SQLITE_NULL_TERMINATED, & loadFilesStmt, nullptr);

        sqlite3_bind_int64(loadFilesStmt, 1, job->GetID());
    }



    while (sqlite3_step(loadFilesStmt) == SQLITE_ROW) {
        // SQLite returns unsigned char * (https://stackoverflow.com/a/804131)

        auto file = std::make_shared<File> (File(
            std::string(reinterpret_cast<const char*>(sqlite3_column_text(loadFilesStmt, 0))), // path
            sqlite3_column_int64(loadFilesStmt, 1), // size
            sqlite3_column_int64(loadFilesStmt, 2)  // mtime
        ));

        dir.AddFilesystemEntity(file);
    }

    sqlite3_finalize(loadFilesStmt);
    sqlite3_close(db);
    return dir;
}

int64_t SQLiteConfigProvider::getLastSnapshotId (const BackupJob* job) {
    sqlite3* db = openDB();
    int64_t snapshotID;
    sqlite3_stmt* getSnapshotStmt;
    sqlite3_prepare_v2(db,
       "select snapshot_id from snapshots where backup_id = ? order by snapshot_id desc limit 1;",
       SQLITE_NULL_TERMINATED, & getSnapshotStmt, nullptr);
    sqlite3_bind_int64(getSnapshotStmt, 1, job->GetID());
    if (sqlite3_step(getSnapshotStmt) == SQLITE_ROW)
        snapshotID = sqlite3_column_int64(getSnapshotStmt, 0);  // mtime
    else
        snapshotID = -1;

    sqlite3_finalize(getSnapshotStmt);
    sqlite3_close(db);
    return snapshotID;
}

sqlite3* SQLiteConfigProvider::openDB () {
    if (!configExists())
        initConfig();

    sqlite3* db;

    if (sqlite3_open(getDbPath().c_str(), & db) != SQLITE_OK)
        throw std::runtime_error("Cannot open the database.");

    return db;
}
