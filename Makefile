CXX=g++
LD=g++
CXXFLAGS=-std=c++17 -Wall -pedantic
FILES=src/bin/intermediate/main.o src/bin/intermediate/FilesystemEntity.o src/bin/intermediate/File.o src/bin/intermediate/Directory.o \
	src/bin/intermediate/Link.o src/bin/intermediate/BackupJob.o src/bin/intermediate/BackupPlan.o src/bin/intermediate/TimedBackupJob.o \
	src/bin/intermediate/ConfigProvider.o src/bin/intermediate/SQLiteConfigProvider.o src/bin/intermediate/FilesystemBrowser.o \
	src/bin/intermediate/Backup.o src/bin/intermediate/BackupStorageProvider.o src/bin/intermediate/IncrementalFilesystemBackupStorageProvider.o \
	src/bin/intermediate/FilesystemBackupStorageProvider.o src/bin/intermediate/UserInterface.o src/bin/intermediate/TerminalUserInterface.o
LIBS=-lstdc++fs

.PHONY: all clean run doc

all: compile doc

doc:
	doxygen

compile: backwither
	cp src/bin/backwither kostada2

run: backwither
	src/bin/backwither

backwither: $(FILES)
	$(LD) -o src/bin/$@ $^ $(LIBS)

clean:
	rm -f  src/bin/backwither
	rm -rf src/bin/intermediate/*
	rm -f  kostada2
	rm -rf doc

src/bin/intermediate/%o: src/%cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
