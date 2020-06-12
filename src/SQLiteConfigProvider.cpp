#include <cstdlib>
#include <stdexcept>
#include <filesystem>
#include <sqlite3.h>
#include <memory>
#include "SQLiteConfigProvider.h"
#include "ChunkListIterator.h"
#include "SQLiteBackupIndexProvider.h"
#include "TimedBackupJob.h"
#include "TimeUtils.h"

SQLiteConfigProvider::SQLiteConfigProvider (std::string path) {
    if (!path.empty())
        if (path[path.length()] != '/')
            path += '/';
    m_Path = path;
    m_DB   = openDB();
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
            path += "/" + std::string(CONFIG_DEFAULT_PATH);
        }
    }

    path += std::string(CONFIG_FOLDER_FILENAME);
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
    "pragma encoding     = 'UTF-8';"
        "pragma foreign_keys = 1;"

        "create table settings (key text unique, value);"

        "create table backups (backup_id integer primary key asc, source text,"
        "destination text, name text unique, compressed integer, "
        "planday integer, plantime integer);"

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
    if (sqlite3_open(getDbPath().c_str(), & db) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }

    sqlite3_stmt* checkTblStmt;
    sqlite3_prepare_v2(db,
        "select 1 from sqlite_master where type = 'table' and name = 'settings';",
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
     sqlite3_stmt* addJobStmt;

    sqlite3_prepare_v2(m_DB,
       "insert into backups (source, destination, name, compressed) values (?, ?, ?, ?);",
       SQLITE_NULL_TERMINATED, & addJobStmt, nullptr);

    // SQLITE_TRANSIENT: SQLite needs to make a copy of the string
    sqlite3_bind_text(addJobStmt, 1, job->GetSource().c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);
    sqlite3_bind_text(addJobStmt, 2, job->GetDestination().c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);
    sqlite3_bind_text(addJobStmt, 3, job->GetName().c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);
    sqlite3_bind_int(addJobStmt, 4, static_cast<int>(job->IsCompressed()));

    if (sqlite3_step(addJobStmt) != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(m_DB);
        sqlite3_finalize(addJobStmt);
        throw std::runtime_error(err+ "Error when adding a new backup job. Name might already be taken.");

    }

    sqlite3_finalize(addJobStmt);
}

BackupJob* SQLiteConfigProvider::GetBackupJob (std::string name) {
    sqlite3_stmt* getBackupJobStmt;
    BackupJob* job;
    sqlite3_prepare_v2(m_DB,
        "select source, destination, name, compressed, backup_id, planday, plantime from backups where name = ?;",
        SQLITE_NULL_TERMINATED, & getBackupJobStmt, nullptr);

    // SQLITE_TRANSIENT: SQLite needs to make a copy of the string
    sqlite3_bind_text(getBackupJobStmt, 1, name.c_str(), SQLITE_NULL_TERMINATED, SQLITE_TRANSIENT);

    if (sqlite3_step(getBackupJobStmt) == SQLITE_ROW) {
        // SQLite returns unsigned char * (https://stackoverflow.com/a/804131)
        std::string src  = std::string(reinterpret_cast<const char*>(sqlite3_column_text(getBackupJobStmt, 0)));
        std::string dst  = std::string(reinterpret_cast<const char*>(sqlite3_column_text(getBackupJobStmt, 1)));
        std::string name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(getBackupJobStmt, 2)));
        bool compressed  = static_cast<bool>(sqlite3_column_int(getBackupJobStmt, 3));
        int64_t id       = sqlite3_column_int64(getBackupJobStmt, 4);
        int planDay      = sqlite3_column_int(getBackupJobStmt, 5);
        int planTime     = sqlite3_column_int(getBackupJobStmt, 6);

        if (planDay)
            job = new TimedBackupJob(src, dst, name, compressed,
                                     static_cast<TimeUtils::weekday_t>(planDay), planTime, id);
        else
            job = new BackupJob(src, dst, name, compressed, id);
        sqlite3_finalize(getBackupJobStmt);
        return job;
    } else {
        sqlite3_finalize(getBackupJobStmt);
        return nullptr;
    }

}

BackupPlan* SQLiteConfigProvider::LoadBackupPlan () {
    BackupPlan* plan = new BackupPlan();
    sqlite3_stmt* loadPlanStmt;
    BackupJob* job = nullptr;

    sqlite3_prepare_v2(m_DB,
       "select source, destination, name, compressed, backup_id, planday, plantime from backups order by name;",
       SQLITE_NULL_TERMINATED, & loadPlanStmt, nullptr);
    while (sqlite3_step(loadPlanStmt) == SQLITE_ROW) {
        // SQLite returns unsigned char * (https://stackoverflow.com/a/804131)
        std::string src  = std::string(reinterpret_cast<const char*>(sqlite3_column_text(loadPlanStmt, 0)));
        std::string dst  = std::string(reinterpret_cast<const char*>(sqlite3_column_text(loadPlanStmt, 1)));
        std::string name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(loadPlanStmt, 2)));
        bool compressed  = static_cast<bool>(sqlite3_column_int(loadPlanStmt, 3));
        int64_t id       = sqlite3_column_int64(loadPlanStmt, 4);
        int planDay      = sqlite3_column_int(loadPlanStmt, 5);
        int planTime     = sqlite3_column_int(loadPlanStmt, 6);

        if (planDay)
            job = new TimedBackupJob(src, dst, name, compressed,
                                     static_cast<TimeUtils::weekday_t>(planDay), planTime, id);
        else
            job = new BackupJob(src, dst, name, compressed, id);

        BackupIndexProvider* indexProvider = nullptr;
        try {
            indexProvider = new SQLiteBackupIndexProvider(job);
            long long completion = indexProvider->LastSuccessfulCompletion();
            job->m_LastCompleted = completion;
        } catch (std::runtime_error & e) {
            job->m_LastCompleted = -2;
        }

        delete indexProvider;

        plan->AddBackup(job);
    }

    sqlite3_finalize(loadPlanStmt);
    return plan;
}

sqlite3* SQLiteConfigProvider::openDB () {
    if (!configExists())
        initConfig();

    sqlite3* db;

    if (sqlite3_open(getDbPath().c_str(), & db) != SQLITE_OK)
        throw std::runtime_error("Cannot open the database.");

    int result = sqlite3_exec(db,
        "pragma encoding     = 'UTF-8';"
            "pragma foreign_keys = 1;",
            nullptr, nullptr, nullptr);
    if (result != SQLITE_OK)
        throw std::runtime_error("Cannot set database parameters.");

    return db;
}

SQLiteConfigProvider::~SQLiteConfigProvider () {
    sqlite3_close(m_DB);
}
