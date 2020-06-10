#include "TimeFileComparator.h"

Directory TimeFileComparator::CompareDirs (Directory prev, Directory curr) {
    /*
    TimeFileComparator c;
    Directory a("/");
    Directory b("/");
    a.AddFilesystemEntity(std::make_shared<File>(File("a", 100, 0)));
    a.AddFilesystemEntity(std::make_shared<File>(File("b", 100, 0)));
    a.AddFilesystemEntity(std::make_shared<File>(File("c", 100, 0)));
    a.AddFilesystemEntity(std::make_shared<File>(File("d", 100, 0)));

    b.AddFilesystemEntity(std::make_shared<File>(File("a", 200, 0)));
    b.AddFilesystemEntity(std::make_shared<File>(File("c", 100, 0)));
    b.AddFilesystemEntity(std::make_shared<File>(File("d", 200, 0)));
    b.AddFilesystemEntity(std::make_shared<File>(File("e", 100, 0)));
    b.AddFilesystemEntity(std::make_shared<File>(File("f", 100, 0)));

         Directory modified = c.CompareDirs(a, b);

     */

    Directory changedFiles("/");
    Directory newFiles = curr - prev;
    Directory deletedFiles = prev - curr;

    // files that appear in both directory states, with old timestamps
    Directory potentiallyChangedFilesPrevious = prev - deletedFiles - newFiles;
    // identical list of files, but with new timestamp to compare
    Directory potentiallyChangedFilesCurrent = curr - deletedFiles - newFiles;

    DirectoryIterator prevIt(& potentiallyChangedFilesPrevious);
    DirectoryIterator currIt(& potentiallyChangedFilesCurrent);

    // std::set is guaranteed to be sorted
    while (!prevIt.End() && !currIt.End()) {
        if (currIt.GetMtime() > prevIt.GetMtime())
            changedFiles.AddFilesystemEntity(currIt.Current());
        currIt++;
        prevIt++;
    }

    return changedFiles + newFiles;
    // todo current problem: hashes are bound to a snapshot and are not saved when file is not modified.
    // possible solution: in index provider, select chunks, that are in snapshot <= that snapshot we are restoring.
    // that way, we ensure that files are restored in their latest possible state
}

TimeFileComparator::TimeFileComparator () {}