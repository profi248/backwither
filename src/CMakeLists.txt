cmake_minimum_required(VERSION 3.16)
project(backwither)

set(CMAKE_CXX_STANDARD 14)

include_directories(..)

add_executable(backwither
        main.cpp FilesystemEntity.cpp File.cpp Directory.cpp Link.cpp
        main.h FilesystemEntity.h File.h Directory.h Link.h BackupJob.cpp
        BackupJob.h BackupPlan.cpp BackupPlan.h TimedBackupJob.cpp TimedBackupJob.h
        ConfigProvider.cpp ConfigProvider.h SQLiteConfigProvider.cpp SQLiteConfigProvider.h
        FilesystemBrowser.cpp FilesystemBrowser.h Backup.cpp Backup.h BackupStorageProvider.cpp
        BackupStorageProvider.h IncrementalFilesystemBackupStorageProvider.cpp IncrementalFilesystemBackupStorageProvider.h
        FilesystemBackupStorageProvider.cpp FilesystemBackupStorageProvider.h UserInterface.cpp UserInterface.h TerminalUserInterface.cpp TerminalUserInterface.h)
