#include <algorithm>
#include <fstream>
#include <filesystem>
#include "BackupJob.h"
#include "FilesystemUtils.h"
#include "FileChunker.h"
#include "SQLiteBackupIndexProvider.h"
#include "TimeFileComparator.h"
#include "TimeUtils.h"

namespace fs = std::filesystem;

// todo handle multiple instances running on one folder (locking)
BackupJob::BackupJob (std::string source, std::string destination, std::string name, bool compressed, int64_t id,
                      long long lastFinished) :
        m_SourcePath (std::move(source)),
        m_DestinationPath (std::move(destination)),
        m_Name (std::move(name)),
        m_Compressed (compressed),
        m_ID (id),
        m_LastCompleted (0) {}

int BackupJob::Backup (UserInterface* ui) {
    fs::path source = fs::path(GetSource());
    fs::path destination = fs::path(GetDestination());

    FilesystemUtils::VerifySourceDirectory(source);
    FilesystemUtils::VerifyOrCreateDestinationDirectory(destination);
    if (FilesystemUtils::ArePathsEqual(source, destination))
        throw std::runtime_error("Backup source and destination lead to the same path.");

    // fixme with make_unique destructor gets called and I don't know why
    std::unique_ptr<BackupIndexProvider> config = std::unique_ptr<SQLiteBackupIndexProvider>
            (new SQLiteBackupIndexProvider(this));

    if (ui)
        ui->UpdateProgress(0, 0, "starting backup", 0);

    Directory prevState = config->LoadSnapshotFileIndex(0);
    Directory currentState = FilesystemUtils::BrowseFolderRecursive(source);
    // maybe implement ignores

    int64_t newSnapshotId = config->SaveSnapshotFileIndex(currentState);

    TimeFileComparator comp;
    Directory toBackup = comp.CompareDirs(prevState, currentState);
    DirectoryIterator it(& toBackup);

    size_t cnt = 1;

    while (!it.End()) {
        if (ui)
            ui->UpdateProgress(cnt, currentState.EntityCount(), it.GetPath(), it.GetSize());
        // if (FilesystemUtils::ArePathsEqual(it.GetPath(), destination))
        //    throw std::runtime_error("Link in source directory (" + it.GetPath() + ") leads to destination directory");
        FileChunker::SaveFileChunks(source / it.GetPath(), it.GetID(), destination,
                                    newSnapshotId, config, IsCompressed());
        it++;
        cnt++;
    }

    config->FinalizeBackup(this);
    return 0;
}

// check if backup is compressed when having identical destination folders for more backups
int BackupJob::Restore (UserInterface* ui, int64_t snapshotId, std::string filePath) {
    fs::path restoreFrom = fs::path(GetDestination());
    fs::path restoreTo = fs::path(GetSource());

    // todo change the verification here, this is trash - maybe refactor
    FilesystemUtils::VerifySourceDirectory(restoreFrom);
    FilesystemUtils::VerifyOrCreateDestinationDirectory(restoreTo);
    if (FilesystemUtils::ArePathsEqual(restoreFrom, restoreTo))
        throw std::runtime_error("Backup source and destination lead to the same path.");

    std::unique_ptr<BackupIndexProvider> config =  std::unique_ptr<SQLiteBackupIndexProvider>
            (new SQLiteBackupIndexProvider(this));

    Directory snapshotFiles = config->LoadSnapshotFileIndex(snapshotId);

    DirectoryIterator it(& snapshotFiles);
    size_t cnt = 1;
    size_t total;
    if (!filePath.empty())
        total = snapshotFiles.EntityCount();
    else
        total = 1;
    while (!it.End()) {
        if (!filePath.empty() && filePath != it.GetPath()) {
            it++;
            continue;
        }
        if (ui)
            ui->UpdateProgress(cnt, total, it.GetPath(), it.GetSize());
        ChunkList fileChunks = config->RetrieveFileChunks(this, snapshotId, it.GetID());
        FileChunker::RestoreFileFromChunks(restoreFrom, restoreTo, fileChunks,
                                               restoreTo / it.GetPath(), IsCompressed());
        it++;
        cnt++;
    }

    return 0;
}

std::string BackupJob::GetSource () const {
    return m_SourcePath;
}

std::string BackupJob::GetDestination () const {
    return m_DestinationPath;
}

std::string BackupJob::GetName () const {
    return m_Name;
}

bool BackupJob::IsCompressed () const {
    return m_Compressed;
}

int64_t BackupJob::GetID () const {
    return m_ID;
}

std::string BackupJob::GetPlan () const {
    return TimeUtils::ZERO;
}

bool BackupJob::ShouldStartBackup () const {
    return false;
}
