cmake_minimum_required(VERSION 3.16)
project(backwither)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)


add_executable(backwither
        main.cpp Filesystem/File.cpp Filesystem/Directory.cpp
        Filesystem/FilesystemEntity.h Filesystem/File.h Filesystem/Directory.h Backup/BackupJob.cpp
        Backup/BackupJob.h Backup/BackupPlan.cpp Backup/BackupPlan.h Backup/TimedBackupJob.cpp Backup/TimedBackupJob.h
        Backup/ConfigProvider.h Backup/SQLiteConfigProvider.cpp Backup/SQLiteConfigProvider.h
        Filesystem/FilesystemUtils.cpp Filesystem/FilesystemUtils.h
        Filesystem/ChunkStorageProvider.h Filesystem/FilesystemChunkStorageProvider.cpp Filesystem/FilesystemChunkStorageProvider.h
        UI/UserInterface.cpp UI/UserInterface.h UI/TerminalUserInterface.cpp UI/TerminalUserInterface.h
        Filesystem/DirectoryComparator.h Filesystem/TimeDirectoryComparator.cpp Filesystem/TimeDirectoryComparator.h
        Iterators/BackupPlanIterator.cpp Iterators/BackupPlanIterator.h Iterators/DirectoryIterator.cpp Iterators/DirectoryIterator.h
        Filesystem/FileChunker.cpp Filesystem/FileChunker.h Backup/Chunk.cpp Backup/Chunk.h Backup/ChunkList.cpp Backup/ChunkList.h Backup/SQLiteBackupIndexProvider.cpp Backup/SQLiteBackupIndexProvider.h
        Iterators/ChunkListIterator.cpp Iterators/ChunkListIterator.h Filesystem/CompressedFilesystemChunkStorageProvider.cpp Filesystem/CompressedFilesystemChunkStorageProvider.h Backup/BackupIndexProvider.h Backup/SnapshotList.cpp Backup/SnapshotList.h Backup/Snapshot.h Backup/Snapshot.cpp Iterators/SnapshotListIterator.cpp Iterators/SnapshotListIterator.h Iterators/SimpleIterator.h Backup/TimeUtils.cpp Backup/TimeUtils.h Filesystem/BasicDirectoryComparator.h Iterators/DirectoryDiffIterator.cpp Iterators/DirectoryDiffIterator.h)

target_link_libraries(backwither -lsqlite3 -lcrypto -lz -lstdc++fs)
