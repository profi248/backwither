#include <algorithm>
#include <fstream>
#include <filesystem>
#include <utility>
#include "BackupJob.h"
#include "FilesystemUtils.h"
#include "FileChunker.h"
#include "UserInterface.h"
#include "SQLiteBackupIndexProvider.h"

namespace fs = std::filesystem;

BackupJob::BackupJob (std::string source, std::string destination, std::string name, bool compressed, int64_t id) :
        m_SourcePath (std::move(source)),
        m_DestinationPath (std::move(destination)),
        m_Name (std::move(name)),
        m_Compressed (compressed),
        m_ID (id) {}

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
    // Directory diff = currentState - prevState;
    // todo improve
    // add file comparator
    // implement ignores
    // handle interruption in the middle of backup (mark snapshot as incomplete)

    int64_t newSnapshotId = config->SaveSnapshotFileIndex(currentState);

    DirectoryIterator it(& currentState);
    size_t cnt = 0;

    while (!it.End()) {
        if (ui)
            ui->UpdateProgress(cnt, 0, it.GetPath(), it.GetSize());
        // if (FilesystemUtils::ArePathsEqual(it.GetPath(), destination))
        //    throw std::runtime_error("Link in source directory (" + it.GetPath() + ") leads to destination directory");
        FileChunker::SaveFileChunks(source / it.GetPath(), it.GetID(), destination,
                                    newSnapshotId, config, IsCompressed());
        it++;
        cnt++;
    }

    // do WAL checkpoint https://www.sqlite.org/pragma.html#pragma_wal_checkpoint
    return 0;
}

int BackupJob::Restore (UserInterface* ui, int64_t snapshotId) {
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
    size_t cnt = 0;

    while (!it.End()) {
        if (ui)
            ui->UpdateProgress(cnt, 0, it.GetPath(), it.GetSize());
        ChunkList fileChunks = config->RetrieveFileChunks(this, snapshotId, it.GetID());
        FilesystemUtils::RestoreFileFromChunks(restoreFrom, restoreTo, fileChunks,
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
