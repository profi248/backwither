# based on makefile from proseminar
CXX=g++
LD=g++
BUILDDIR=src/bin
OBJDIR=$(BUILDDIR)/intermediate
CXXFLAGS=-std=c++17 -Wall -pedantic -Wextra -O3
LDFLAGS=-lsqlite3 -lcrypto -lz -lstdc++fs
FILES= \
	$(OBJDIR)/File.o $(OBJDIR)/Directory.o \
	$(OBJDIR)/BackupJob.o $(OBJDIR)/BackupPlan.o $(OBJDIR)/TimedBackupJob.o \
	$(OBJDIR)/SQLiteConfigProvider.o $(OBJDIR)/FilesystemUtils.o \
	$(OBJDIR)/FilesystemChunkStorageProvider.o $(OBJDIR)/CompressedFilesystemChunkStorageProvider.o \
	$(OBJDIR)/UserInterface.o $(OBJDIR)/TerminalUserInterface.o \
	$(OBJDIR)/TimeDirectoryComparator.o $(OBJDIR)/DirectoryDiffIterator.o \
	$(OBJDIR)/BackupPlanIterator.o $(OBJDIR)/DirectoryIterator.o $(OBJDIR)/FileChunker.o \
	$(OBJDIR)/Chunk.o $(OBJDIR)/ChunkList.o $(OBJDIR)/ChunkListIterator.o \
	$(OBJDIR)/SQLiteBackupIndexProvider.o $(OBJDIR)/Snapshot.o \
	$(OBJDIR)/SnapshotList.o $(OBJDIR)/SnapshotListIterator.o $(OBJDIR)/TimeUtils.o

.PHONY: all clean run doc prepare

all: compile doc

test: CXXFLAGS += -fsanitize=address -g
test: LDFLAGS := -lasan $(LDFLAGS) # AddressSanitizer needs to be linked first
debug: CXXFLAGS += -fsanitize=address -g
debug: LDFLAGS := -lasan $(LDFLAGS) # AddressSanitizer needs to be linked first

debug-noasan:  CXXFLAGS += -g

doc:
	doxygen

debug: prepare backwither
debug-noasan: prepare backwither

compile: prepare backwither
	cp src/bin/backwither kostada2

prepare:
	mkdir -p src/bin/intermediate > /dev/null 2>&1

run: prepare backwither
	src/bin/backwither

test: $(FILES) | prepare
	$(CXX) $(CXXFLAGS) -c -o $(OBJDIR)/testbench.o src/tests/testbench.cpp
	$(LD) -o src/bin/test $^ $(OBJDIR)/testbench.o $(LDFLAGS)
	src/bin/test

backwither: $(FILES) $(OBJDIR)/main.o
	$(LD) -o src/bin/$@ $^ $(LDFLAGS)

clean:
	rm -r  $(BUILDDIR)/backwither
	rm -rf $(OBJDIR)/*
	rm -f  kostada2
	rm -rf doc

$(OBJDIR)/%o: src/%cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
