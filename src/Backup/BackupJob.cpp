#include <algorithm>
#include <fstream>
#include <filesystem>
#include "BackupJob.h"
#include "SQLiteBackupIndexProvider.h"
#include "TimeUtils.h"
#include "../Filesystem/FilesystemUtils.h"
#include "../Filesystem/FileChunker.h"
#include "../Filesystem/TimeDirectoryComparator.h"
#include "../Filesystem/BasicDirectoryComparator.h"

namespace fs = std::filesystem;

BackupJob::BackupJob (std::string source, std::string destination, std::string name, bool compressed, int64_t id) :
        m_SourcePath (std::move(source)),
        m_DestinationPath (std::move(destination)),
        m_Name (std::move(name)),
        m_Compressed (compressed),
        m_ID (id) {}

void BackupJob::Backup (UserInterface* ui, bool disableTimeComp) {
    fs::path source = fs::path(GetSource());
    fs::path destination = fs::path(GetDestination());

    FilesystemUtils::VerifySourceDirectory(source);
    FilesystemUtils::VerifyOrCreateDestinationDirectory(destination);
    if (FilesystemUtils::ArePathsEqual(source, destination))
        throw std::runtime_error("Backup source and destination lead to the same path.");

    std::unique_ptr<BackupIndexProvider> config = std::unique_ptr<SQLiteBackupIndexProvider>
            (new SQLiteBackupIndexProvider(this));

    if (m_Compressed != config->GetCompressed())
        throw std::runtime_error("Compression setting does not match with compression of destination folder.");

    if (ui)
        ui->UpdateProgress(0, 0, "starting backup", 0);

    Directory prevState = config->LoadSnapshotFileIndex(0);
    Directory currentState = FilesystemUtils::BrowseFolderRecursive(source);
    // maybe implement ignores

    int64_t newSnapshotId = config->SaveSnapshotFileIndex(currentState);

    DirectoryComparator* comp = nullptr;
    if (!disableTimeComp)
        comp = new TimeDirectoryComparator();
    else
        comp = new BasicDirectoryComparator();

    Directory toBackup = comp->CompareDirs(prevState, currentState);
    delete comp;
    DirectoryIterator it(& toBackup);

    size_t cnt = 1;

    while (!it.End()) {
        if (ui)
            ui->UpdateProgress(cnt, currentState.EntityCount(), it.GetPath(), it.GetSize());
        FileChunker::SaveFileChunks(source / it.GetPath(), it.GetID(), destination,
                                    newSnapshotId, config, IsCompressed());
        it++;
        cnt++;
    }

    config->FinalizeBackup();
}

void BackupJob::Restore (UserInterface* ui, int64_t snapshotId, std::string filePath) {
    fs::path restoreFrom = fs::path(GetDestination());
    fs::path restoreTo = fs::path(GetSource());

    FilesystemUtils::VerifySourceDirectory(restoreFrom);
    FilesystemUtils::VerifyOrCreateDestinationDirectory(restoreTo);
    if (FilesystemUtils::ArePathsEqual(restoreFrom, restoreTo))
        throw std::runtime_error("Backup source and destination lead to the same path.");

    std::unique_ptr<BackupIndexProvider> config = std::unique_ptr<SQLiteBackupIndexProvider>
            (new SQLiteBackupIndexProvider(this));

    if (m_Compressed != config->GetCompressed())
        throw std::runtime_error("Compression setting does not match with compression of destination folder.");

    Directory snapshotFiles = config->LoadSnapshotFileIndex(snapshotId);

    DirectoryIterator it(& snapshotFiles);
    size_t cnt = 1;
    size_t total;
    if (filePath.empty())
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
        ChunkList fileChunks = config->RetrieveFileChunks(snapshotId, it.GetID());
        FileChunker::RestoreFileFromChunks(restoreFrom, fileChunks,
                                           restoreTo / it.GetPath(), IsCompressed());
        if (!filePath.empty())
            break;
        it++;
        cnt++;
    }
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

TimeUtils::weekday_t BackupJob::GetPlanWeekday () const {
    return TimeUtils::NONE;
}

int BackupJob::GetPlanSecsSinceDay () const {
    return -1;
}

long long BackupJob::GetLastCompleted () const {
    return m_LastCompleted;
}

void BackupJob::SetLastCompleted (long long timestamp) {
    m_LastCompleted = timestamp;
}
