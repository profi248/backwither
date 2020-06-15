#include <getopt.h>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <unistd.h>

#include "TerminalUserInterface.h"
#include "SQLiteConfigProvider.h"
#include "SQLiteBackupIndexProvider.h"
#include "FilesystemUtils.h"
#include "SnapshotListIterator.h"
#include "TimeDirectoryComparator.h"
#include "TimeUtils.h"
#include "TimedBackupJob.h"
#include "DirectoryDiffIterator.h"

const int TERM_RED = 31;
const int TERM_GREEN = 32;
const int TERM_BLUE = 34;

using namespace std;

int TerminalUserInterface::StartInterface (int argc, char** argv) {
    // example from documentation (https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html#Using-Getopt)
    opterr = 0;

    char *id = nullptr, *source = nullptr, *destination = nullptr, *time = nullptr, *wday = nullptr;
    char *name = nullptr, *comparePair = nullptr, *filePath = nullptr;
    bool compress = true, disableTimeComparator = false;
    int  index, c;

    // parse arguments: options need to be parsed first, then commands

    while ((c = getopt (argc, argv, "i:s:d:n:c:p:f:t:w:xm")) != -1)
        switch (c) {
            case 'i':
                id = optarg;
                break;
            case 's':
                source = optarg;
                break;
            case 'f':
                filePath = optarg;
                break;
            case 'd':
                destination = optarg;
                break;
            case 'p':
                comparePair = optarg;
                break;
            case 'n':
                name = optarg;
                break;
            case 'c':
                m_ConfigPath = optarg;
                break;
            case 'w':
                wday = optarg;
                break;
            case 't':
                time = optarg;
                break;
            case 'x':
                compress = false;
                break;
            case 'm':
                disableTimeComparator = true;
                break;
            case '?':
                if (optopt == 'f' || optopt == 'p' || optopt == 'i' || optopt == 's' || optopt == 't' ||
                    optopt == 'd' || optopt == 'n' || optopt == 'c' || optopt == 'w' )
                    cerr << "Option -" << (char) optopt << " requires an argument." << endl;
                else
                    cerr << "Unknown option -" << (char) optopt << "." << endl;
                return 1;
            default:
                throw bad_exception();
        }

    for (index = optind; index < argc; index++) {
        string command(argv[index]);
        // apparently we can't use switch for strings...
        if (command == "list") {
            return list();
        } else if (command == "history") {
            if (!name) {
                cerr << "Specifying backup name (-n) is required." << endl;
                return 1;
            }

            return history(name);
        } else if (command == "add") {
            if (!source || !destination || !name) {
                cerr << "Specifying backup source path (-s), destination path (-d) and name (-n) is required." << endl;
                return 1;
            }

            if ((wday && !time) || (!wday && time)) {
                cerr << "For a scheduled backup, both weekday (-w) and time (-t) is required." << endl;
                return 1;
            }

            return add(source, destination, name, compress, wday, time);
        } else if (command == "backup") {
            if (!name) {
                cerr << "Specifying backup name (-n) is required." << endl;
                return 1;
            }

            return backup(name, disableTimeComparator);
        } else if (command == "restore") {
            if (!name) {
                cerr << "Specifying backup name (-n) is required." << endl;
                return 1;
            }
            int64_t snapshotId = 0;
            if (id)
                snapshotId = strtoll(id, nullptr, 10);

            return restore(name, snapshotId, filePath);
        } else if (command == "diff") {
            if (!comparePair | !name) {
                cerr << "Specifying backup name (-n) and snapshot comparison pair (-p) is required." << endl;
                return 1;
            }


            try {
                auto snapshotIDs = TimeUtils::ParsePosColumnSeparatedInts(comparePair);
                return diff(name, snapshotIDs.first, snapshotIDs.second, filePath);
            } catch (invalid_argument & e) {
                cerr << e.what() << endl;
                return 1;
            }

        } else if (command == "show") {
            if (!id | !name) {
                cerr << "Specifying backup name (-n) and snapshot ID (-i) is required." << endl;
                return 1;
            }

            int64_t snapshotId = 0;
            snapshotId = strtoll(id, nullptr, 10);

            return show(name, snapshotId);
        } else if (command == "run-all") {
            return runAll();
        } else if (command == "run-cron") {
            return runCron();
        } else if (command == "remove") {
            if (!name) {
                cerr << "Specifying backup name (-n) is required." << endl;
                return 1;
            }

            return remove(name);
        } else if (command == "help") {
            return help();
        } else {
            cerr << "Command " << command << " not recognized." << endl;
            help();
            return 1;
        }
    }
    // default command
    help();
    return 0;
}

int TerminalUserInterface::list () {
    // show that destination is inaccessible
    BackupPlan* plan = loadBackupPlan();
    if (!plan)
        return 2;

    BackupPlanIterator* it = new BackupPlanIterator(plan);

    if (it->Empty()) {
        cout << "No backup jobs. Add a new job by running `add`." << endl
             << "If you expected to see something, please verify that correct config is in place." << endl;

        delete it;
        delete plan;
        return 0;
    }

    printTable(it);

    delete it;
    delete plan;
    return 0;
}

int TerminalUserInterface::history (char* backupName) {
    BackupIndexProvider* indexProvider = nullptr;
    SnapshotList* snapshots = nullptr;

    BackupJob* job = findBackupJobByName(backupName);
    if (!job)
        return 2;

    try {
        if (!std::filesystem::exists(job->GetDestination())) {
            cerr << "Backup destination folder " << job->GetDestination() << " not found. Maybe no backups have been run yet." << endl;
            delete job;
            return 2;
        }

        indexProvider = new SQLiteBackupIndexProvider(job);
        snapshots = indexProvider->LoadSnapshotList();
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete job;
        delete snapshots;
        delete indexProvider;
        return 2;
    }

    SnapshotListIterator* it = new SnapshotListIterator(snapshots);

    if (it->Empty()) {
        cout << "No backups run yet." << endl;

        delete it;
        delete job;
        delete snapshots;
        delete indexProvider;
        return 0;
    }

    printTable(it);

    delete it;
    delete job;
    delete snapshots;
    delete indexProvider;
    return 0;
}

ConfigProvider* TerminalUserInterface::getConfigProvider () const {
    ConfigProvider* config;

    if (!m_ConfigPath.empty())
        config = new SQLiteConfigProvider(m_ConfigPath);
    else
        config = new SQLiteConfigProvider();

    return config;
}

int TerminalUserInterface::help () {
    cout << "backwither " << getVersion() << endl << endl;
    cout << "Help / Usage" << endl << endl;
    cout << "Commands" << endl;
    cout << "  list\t\tshow backup jobs" << endl <<
            "  add\t\tadd new backup job (optionally with schedule)" << endl <<
            "  remove\tremove backup job" << endl <<
            "  backup\trun backup job" << endl <<
            "  restore\trestore backup or a single file (optionally specify snapshot with -i)" << endl <<
            "  diff\t\tshow difference between snapshots (specify snapshots to compare with -p)" << endl <<
            "  show\t\tshow files in specified snapshot" << endl <<
            "  history\tshow snapshots in a specified backup job" << endl <<
            "  run-all\trun all backups" << endl <<
            "  run-cron\trun planned backups due for a run" << endl <<
            "  help\t\tshow this help message" << endl << endl;

    cout << "Options" << endl;

    cout << "  -c\tspecify config directory" << endl <<
            "  -d\tspecify new backup job destination path" << endl <<
            "  -f\tspecify a path to a specific file (for restore or diff)" << endl <<
            "  -i\tspecify snapshot ID (to restore)" << endl <<
            "  -m\tdisable filesystem time comparision speedup of backup" << endl <<
            "  -n\tspecify backup job name" << endl <<
            "  -p\tspecify a pair of snapshot to compare in diff [format id:id]" << endl <<
            "  -s\tspecify new backup job source path" << endl <<
            "  -t\tspecify a time to run a backup (when adding a new backup) [format 00:00-23:59]" << endl <<
            "  -w\tspecify a day of week to run a backup (when adding a new backup) [format mo-su]" << endl <<
            "  -x\tdisable compression (when adding a new backup)" << endl;
    return 0;
}

string TerminalUserInterface::getVersion () {
    return "dev";
}

int TerminalUserInterface::add (char* source, char* destination, char* name, bool compress, char* wday, char* time) {
    ConfigProvider* config = getConfigProvider();

    BackupJob* job = nullptr;
    try {
        std::string src = FilesystemUtils::AbsolutePath(source);
        std::string dst = FilesystemUtils::AbsolutePath(destination, true);
        if (FilesystemUtils::ArePathsEqual(src, dst))
            throw std::runtime_error("Source and destination leads to the same path.");

        if (wday && time) {
            TimeUtils::weekday_t weekday = TimeUtils::StringToWeekday(wday);
            if (weekday == TimeUtils::NONE) {
                cerr << "Invalid weekday. Type mo, tu, we, th, fr, sa or su." << endl;
                delete job;
                delete config;
                return 1;
            }
            int secsSinceStart = -1;

            try {
                secsSinceStart = TimeUtils::StringToUTCSecondsSinceStart(time);
            } catch (invalid_argument & e) {
                cerr << e.what() << endl;
                delete job;
                delete config;
                return 1;
            }

            if (secsSinceStart == -1) {
                cerr << "Hours or minutes are out of range." << endl;
                delete job;
                delete config;
                return 1;
            }
            job = new TimedBackupJob(src, dst, name, compress, weekday, secsSinceStart);

        } else {
            job = new BackupJob(src, dst, name, compress);
        }


        config->AddBackupJob(job);
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete job;
        delete config;
        return 2;
    }

    delete job;
    delete config;
    return 0;
}

int TerminalUserInterface::backup (char* name, bool disableTimeComparator) {
    // add potential not enough space warning
    BackupJob* job = findBackupJobByName(name);
    if (!job)
        return 2;

    try {
        job->Backup(this, disableTimeComparator);
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete job;
        return 2;
    }

    delete job;
    return 0;
}

// todo better handle incorrect input (nonexistent snapshots/internal values)
int TerminalUserInterface::diff (char* name, int64_t snapshotIdA, int64_t snapshotIdB, char* file) {

    if (snapshotIdA == snapshotIdB) {
        cerr << "Compared snapshot IDs must differ." << endl;
        return 1;
    }

    bool filter = false;
    if (file)
        filter = true;

    BackupIndexProvider* indexProvider = nullptr;
    ConfigProvider* config = getConfigProvider();
    DirectoryIterator *itAdded = nullptr, *itRemoved = nullptr;
    DirectoryDiffIterator *itModified = nullptr;

    BackupJob* job = findBackupJobByName(name);
    if (!job) {
        delete config;
        return 2;
    }

    try {
        // snapshot B should be always older
        if (snapshotIdA < snapshotIdB) {
            int64_t tmp = snapshotIdB;
            snapshotIdB = snapshotIdA;
            snapshotIdA = tmp;
        }

        indexProvider = new SQLiteBackupIndexProvider(job);
        Directory a = indexProvider->LoadSnapshotFileIndex(snapshotIdA);
        Directory b = indexProvider->LoadSnapshotFileIndex(snapshotIdB);
        Directory added = a - b;
        Directory removed = b - a;
        itAdded = new DirectoryIterator(& added);
        itRemoved = new DirectoryIterator(& removed);
        int formatChars;

        TimeDirectoryComparator comp;
        Directory modified = comp.CompareDirs(b, a) - added - removed;
        Directory modifiedB = b - added - removed;
        Directory modifiedA = a - added - removed;
        itModified = new DirectoryDiffIterator (& modifiedB, & modifiedA);

        cout << "comparing snapshots " << snapshotIdA << " and " << snapshotIdB << endl;
        if (itAdded->Empty() && itModified->Empty() && itModified->Empty())
            cout << "no difference" << endl;

        if (!itAdded->Empty()) {
            cout << format("files added", formatChars, true, TERM_GREEN) << endl;
            if (filter)
                printTable(itAdded, 0, std::string(file));
            else
                printTable(itAdded);

            cout << endl;
        }

        if (!itRemoved->Empty()) {
            cout << format("files removed", formatChars, true, TERM_RED) << endl;
            if (filter)
                printTable(itRemoved, 0, std::string(file));
            else
                printTable(itRemoved);
            cout << endl;
        }

        if (!itModified->Empty()) {
            cout << format("files modified", formatChars, true, TERM_BLUE) << endl;
            if (filter)
                printTable(itModified, 0, std::string(file));
            else
                printTable(itModified);
        }

    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete job;
        delete indexProvider;
        delete config;
        delete itAdded;
        delete itRemoved;
        delete itModified;
        return 2;
    }

    delete job;
    delete indexProvider;
    delete config;
    delete itAdded;
    delete itRemoved;
    delete itModified;
    return 0;
}

int TerminalUserInterface::show (char* name, int64_t snapshotId) {
    BackupIndexProvider* indexProvider = nullptr;
    DirectoryIterator* it = nullptr;

    BackupJob* job = findBackupJobByName(name);
    if (!job)
        return 2;

    try {
        indexProvider = new SQLiteBackupIndexProvider(job);
        Directory files = indexProvider->LoadSnapshotFileIndex(snapshotId);
        it = new DirectoryIterator(& files);
        printTable(it);
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete job;
        delete indexProvider;
        delete it;
        return 2;
    }

    delete job;
    delete indexProvider;
    delete it;
    return 0;
}

int TerminalUserInterface::restore (char* name, int64_t snapshotId, char* filePath) {
    string file;

    BackupJob* job = findBackupJobByName(name);
    if (!job)
        return 2;

    BackupIndexProvider* indexProvider = nullptr;
    long long completion;
    try {
        indexProvider = new SQLiteBackupIndexProvider(job);
        completion = indexProvider->GetSnapshot(snapshotId).GetCompletion();
    } catch (std::runtime_error & e) {
        completion = -2;
    }

    delete indexProvider;
    int formattedChars;

    if (completion <= 0) {
        cerr << format("WARNING!", formattedChars) << " Snapshot you are restoring is not completed. Some files might not have been backed up." << endl;
    }

    try {
        if (filePath) {
            file = string(filePath);
            BackupIndexProvider* indexProvider = new SQLiteBackupIndexProvider(job);
            if (!indexProvider->DoesFileExistInSnapshot(snapshotId, file)) {
                cerr << "The file you specified doesn't exist in selected snapshot." << endl;
                delete indexProvider;
                delete job;
                return 1;
            }
            delete indexProvider;
        }
        if (!FilesystemUtils::IsDirectoryEmpty(job->GetSource()) && !filePath) {
            cerr << format("WARNING!", formattedChars) << " Restore directory is not empty. All files in backup source ("
                 << job->GetSource()
                 << ") will be replaced by versions from the snapshot you selected! Continue? [y/N] "
                 << flush;

            if (yesNoPrompt()) {
                job->Restore(this, snapshotId, file);
            } else {
                delete job;
                return 0;
            }

        } else {
            job->Restore(this, snapshotId, file);
        }

    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete job;
        return 2;
    }

    delete job;
    return 0;
}

int TerminalUserInterface::runAll () {
    BackupPlan* plan = loadBackupPlan();
    if (!plan)
        return 2;

    BackupPlanIterator* it = new BackupPlanIterator(plan);

    while (!it->End()) {
        try {
            BackupJob* job = it->Current();
            job->Backup(this, false);
        } catch (runtime_error & e) {
            cerr << "Fatal error: " << e.what() << endl;
            delete it;
            delete plan;
            return 2;
        }
        (*it)++;
    }

    delete it;
    delete plan;
    return 0;
}

int TerminalUserInterface::runCron () {
    BackupPlan* plan = loadBackupPlan();
    if (!plan)
        return 2;

    BackupPlanIterator* it = new BackupPlanIterator(plan);

    while (!it->End()) {
        try {
            BackupJob* job = it->Current();
            if (job->ShouldStartBackup())
                job->Backup(this, false);
        } catch (runtime_error & e) {
            cerr << "Fatal error: " << e.what() << endl;
            delete it;
            delete plan;
            return 2;
        }
        (*it)++;
    }

    delete it;
    delete plan;
    return 0;
}

int TerminalUserInterface::remove (char* name) {
    ConfigProvider* config = getConfigProvider();
    BackupJob* job = findBackupJobByName(name);
    if (!job) {
        delete config;
        return 2;
    }

    try {
        config->DeleteBackupJob(job);
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete config;
        delete job;
        return 2;
    }

    delete config;
    delete job;
    return 0;
}


void TerminalUserInterface::UpdateProgress (size_t current, size_t expected, std::string status, size_t fileSize) {
    if (!isatty(fileno(stderr)) || !ENABLE_PROGRESS)
        return;
    stringstream oss;
    if (expected > 0)
        oss << "[" << current << "/" << expected << "] ";
    else if (current > 0)
        oss << "[" << current << "...] ";
    else
        oss << "[...] ";

    oss << status;
    if (fileSize)
         oss << " (" << UserInterface::HumanFileSize(fileSize) << ")";
    cerr << left << setw(m_LastStatusLength) << setfill(' ') << oss.str() << '\r' << flush;
    m_LastStatusLength = oss.str().length();

}

std::string TerminalUserInterface::format (std::string in, int & formatChars, bool bold, int color) {
    formatChars = 0;
    if (!isatty(fileno(stdout)) || !ENABLE_COLORS)
        return in;

    string prefix = "\033[";
    formatChars += 2;

    if (color > 0) {
        string strCode = to_string(color);
        formatChars += strCode.length();
        prefix += strCode;
    }

    if (bold) {
        formatChars += 1;
        prefix += ";";
        formatChars += 1;
        prefix += "1";
    }

    prefix += "m";
    formatChars += 1 + 4;
    return prefix + in + "\033[0m";
}

bool TerminalUserInterface::yesNoPrompt () {
    string answer;
    while (answer != "n" && answer != "y" && answer != "N" && answer != "Y") {
        getline(cin, answer);

        if (answer == "n" || answer == "N" || answer.empty()) {
            return false;
        } else if (answer == "y" || answer == "Y") {
            return true;
        } else {
            cerr << "Invalid answer. Please only type \"y\" for yes or \"n\" for no. [y/N] " << flush;
        }
    }

    return false;
}

// setw broken in unicode
void TerminalUserInterface::printTable (SimpleIterator* it, size_t filterCol, std::string filterStr) {
    if (!it | it->Empty())
        return;

    int colPadding = 2;

    vector<string> hdrs = it->TableHeader();
    vector<size_t> colHdrWidths;
    colHdrWidths.reserve(hdrs.size());
    vector<size_t> colWidths;


    if (it->TableRow().size() != hdrs.size())
        throw std::logic_error("Number of columns in header doesn't match number of coulumns in row.");

    // compute initial miniumum widths (header lengths)
    for (auto & hdr : hdrs) {
        colHdrWidths.push_back(hdr.length() + colPadding);
    }

    // compute widths of columns with data
    colWidths = colHdrWidths;
    // todo do not print headers if all data is filtered
    while (!it->End()) {
        size_t i = 0;
        for (auto & val : it->TableRow()) {
            if (i != filterCol || (i == filterCol && filterStr == val))
                colWidths[i] = max(colWidths[i], val.length() + colPadding);

            i++;
        }
        (*it)++;
    }

    it->Rewind();

    // print headers
    size_t i = 0;
    int formatChars;
    for (auto & hdr : hdrs) {
        string formatted = format(hdr, formatChars);
        cout << left << setw(colWidths[i++] + formatChars) << formatted;
    }

    cout << endl;
    // print data
    while (!it->End()) {
        bool filtered = false;
        i = 0;
        for (auto & col : it->TableRow()) {
            if (i != filterCol || (i == filterCol && filterStr == col)) {
                cout << left << setw(colWidths[i++]) << col;
            } else {
                filtered = true;
            }
        }
        if (!filtered)
            cout << endl;
        (*it)++;
    }
}

// from https://stackoverflow.com/a/3586973/2465760
size_t TerminalUserInterface::countUtf8Codepoints (std::string in) {
    size_t count = 0;
    for (unsigned char c : in)
        count += ((c & 0xc0u) != 0x80);

    cout << "UTF-8: " << count << " bytes: " << in.length() << endl;
    return count;
}

BackupJob* TerminalUserInterface::findBackupJobByName (char* name) {
    ConfigProvider* config = getConfigProvider();
    BackupJob* job;
    try {
        job = config->GetBackupJob(name);
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete config;
        return nullptr;
    }

    if (!job) {
        cerr << "Backup job named \"" << name << "\" not found." << endl;
        delete config;
        return nullptr;
    }

    delete config;
    return job;
}

BackupPlan* TerminalUserInterface::loadBackupPlan () {
    ConfigProvider* config = nullptr;
    BackupPlan* plan = nullptr;
    try {
        config = getConfigProvider();
        plan = config->LoadBackupPlan();
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete config;
        return nullptr;
    }

    delete config;
    return plan;
}

void TerminalUserInterface::cleanRow () {
    if (!isatty(fileno(stderr)) || !ENABLE_PROGRESS)
        return;
    cerr << left << setw(m_LastStatusLength) << setfill(' ') << '\r' << flush;
}
