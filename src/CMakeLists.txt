cmake_minimum_required(VERSION 3.16)
project(backwither)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)


add_executable(backwither
        main.cpp File.cpp Directory.cpp Link.cpp
        FilesystemEntity.h File.h Directory.h Link.h BackupJob.cpp
        BackupJob.h BackupPlan.cpp BackupPlan.h TimedBackupJob.cpp TimedBackupJob.h
        ConfigProvider.h SQLiteConfigProvider.cpp SQLiteConfigProvider.h
        FilesystemUtils.cpp FilesystemUtils.h
        ChunkStorageProvider.h FilesystemChunkStorageProvider.cpp FilesystemChunkStorageProvider.h
        UserInterface.cpp UserInterface.h TerminalUserInterface.cpp TerminalUserInterface.h
        DirectoryComparator.h TimeDirectoryComparator.cpp TimeDirectoryComparator.h
        BackupPlanIterator.cpp BackupPlanIterator.h DirectoryIterator.cpp DirectoryIterator.h
        FileChunker.cpp FileChunker.h Chunk.cpp Chunk.h ChunkList.cpp ChunkList.h SQLiteBackupIndexProvider.cpp SQLiteBackupIndexProvider.h
        ChunkListIterator.cpp ChunkListIterator.h CompressedFilesystemChunkStorageProvider.cpp CompressedFilesystemChunkStorageProvider.h BackupIndexProvider.h SnapshotList.cpp SnapshotList.h Snapshot.h Snapshot.cpp SnapshotListIterator.cpp SnapshotListIterator.h SimpleIterator.h TimeUtils.cpp TimeUtils.h BasicDirectoryComparator.h DirectoryDiffIterator.cpp DirectoryDiffIterator.h)

target_link_libraries(backwither -lsqlite3 -lcrypto -lz -lstdc++fs)
