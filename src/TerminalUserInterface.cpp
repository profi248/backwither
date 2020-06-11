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
#include "TimeFileComparator.h"

const int TERM_RED = 31;
const int TERM_GREEN = 32;
const int TERM_BLUE = 34;

using namespace std;

const int TABLE_FIELD_LENGTH = 20;
// todo refactor config
int TerminalUserInterface::StartInterface (int argc, char** argv) {
    // example from documentation (https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html#Using-Getopt)

    opterr = 0;

    char* id          = nullptr;
    char* source      = nullptr;
    char* destination = nullptr;
    char* name        = nullptr;
    char* configPath  = nullptr;
    char* comparePair = nullptr;
    char* filePath    = nullptr;
    bool compress     = true;
    int  index;
    int  c;

    // default command is help
    if (argc == 1)
        help();

    // parse arguments: options need to be parsed first, then commands

    while ((c = getopt (argc, argv, "i:s:d:n:c:p:f:x")) != -1)
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
                configPath = optarg;
                break;
            case 'x':
                compress = false;
                break;
            case '?':
                if (optopt == 'f' || optopt == 'p' || optopt == 'i' || optopt == 's' ||
                    optopt == 'd' || optopt == 'n' || optopt == 'c')
                    cerr << "Option -" << (char) optopt << " requires an argument." << endl;
                else
                    cerr << "Unknown option -" << (char) optopt << "." << endl;
                return 1;
            default:
                throw bad_exception();
        }

    // todo change config to member var!
    for (index = optind; index < argc; index++) {
        string command(argv[index]);
        // apparently we can't use switch for strings...
        if (command == "list") {
            list(configPath);
        } else if (command == "history") {
            if (!name) {
                cerr << "Specifying backup name (-n) is required." << endl;
                return 1;
            }

            history(configPath, name);
        } else if (command == "add") {
            if (!source || !destination || !name) {
                cerr << "Specifying backup source path (-s), destination path (-d) and name (-n) is required." << endl;
                return 1;
            }

            add(source, destination, name, configPath, compress);
        } else if (command == "backup") {
            if (!name) {
                cerr << "Specifying backup name (-n) is required." << endl;
                return 1;
            }

            backup(name, configPath);
        } else if (command == "restore") {
            if (!name) {
                cerr << "Specifying backup name (-n) is required." << endl;
                return 1;
            }
            int64_t snapshotId = 0;
            if (id)
                snapshotId = strtoll(id, nullptr, 10);

            restore(name, snapshotId, filePath, configPath);
        } else if (command == "diff") {
            if (!comparePair | !name) {
                cerr << "Specifying backup name (-n) and snapshot comparison pair (-p) is required." << endl;
                return 1;
            }
            string rawSnapshotIdA, rawSnapshotIdB;
            int64_t snapshotIdA, snapshotIdB;
            bool second = false;

            for (char ch : string(comparePair)) {
                if (ch == ':') {
                    second = true;
                    continue;
                }

                if (!second)
                    rawSnapshotIdA += ch;
                else
                    rawSnapshotIdB += ch;
            }

            if (!second) {
                cerr << "Invalid snapshot pair format supplied. Example of a correct format is 4:6." << endl;
                return 1;
            }

            try {
                snapshotIdA = stoll(rawSnapshotIdA);
                snapshotIdB = stoll(rawSnapshotIdB);
            } catch (exception &) {
                cerr << "Supplied snapshot ID is not numeric." << endl;
                return 1;
            }

            if (snapshotIdA == snapshotIdB) {
                cerr << "Compared snapshot IDs must differ." << endl;
                return 1;
            }

            diff(name, snapshotIdA, snapshotIdB, configPath);
        } else if (command == "show") {
            if (!id | !name) {
                cerr << "Specifying backup name (-n) and snapshot ID (-i) is required." << endl;
                return 1;
            }

            int64_t snapshotId = 0;
            snapshotId = strtoll(id, nullptr, 10);

            show(name, snapshotId, configPath);
        } else if (command == "help") {
            help();
        } else {
            cerr << "Command " << command << " not recognized." << endl;
            return 1;
        }
    }

    return 0;
}

int TerminalUserInterface::list (char* configPath) {
    // show that destination is inaccessible
    ConfigProvider* config = getConfigProvider(configPath);

    BackupPlan* plan;
    try {
        plan = config->LoadBackupPlan();
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete config;
        return 2;
    }

    BackupPlanIterator* it = new BackupPlanIterator(plan);

    if (it->Empty()) {
        cout << "No backup jobs. Add a new job by running `add`." << endl
             << "If you expected to see something, please verify that correct config is in place." << endl;

        delete it;
        delete plan;
        delete config;
        return 0;
    }

    printTable(it);

    delete it;
    delete plan;
    delete config;
    return 0;
}

int TerminalUserInterface::history (char* configPath, char* backupName) {
    BackupJob* job = nullptr;
    ConfigProvider* config = nullptr;
    BackupIndexProvider* indexProvider = nullptr;
    SnapshotList* snapshots = nullptr;
    try {
        config = getConfigProvider(configPath);
        job = config->GetBackupJob(backupName);

        if (!job) {
            cerr << "Backup job named \"" << backupName << "\" not found." << endl;
            delete config;
            return 1;
        }

        if (!std::filesystem::exists(job->GetDestination())) {
            cerr << "Backup destination folder " << job->GetDestination() << " not found. Maybe no backups have been run yet." << endl;
            delete config;
            delete job;
            return 2;
        }

        indexProvider = new SQLiteBackupIndexProvider(job);
        snapshots = indexProvider->LoadSnapshotList();
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete job;
        delete snapshots;
        delete config;
        delete indexProvider;
        return 2;
    }

    SnapshotListIterator* it = new SnapshotListIterator(snapshots);

    if (it->Empty()) {
        cout << "No backups run yet." << endl;

        delete it;
        delete job;
        delete snapshots;
        delete config;
        delete indexProvider;
        return 0;
    }

    printTable(it);

    delete it;
    delete job;
    delete snapshots;
    delete config;
    delete indexProvider;
    return 0;
}

ConfigProvider* TerminalUserInterface::getConfigProvider (const char* configPath) const {
    ConfigProvider* config;

    if (configPath)
        config = new SQLiteConfigProvider(configPath);
    else
        config = new SQLiteConfigProvider();

    return config;
}

int TerminalUserInterface::help () {
    cout << "backwither " << getVersion() << endl << endl;
    cout << "Help / Usage" << endl << endl;
    cout << "Commands" << endl;
    cout << "  list\t\tshow backup jobs" << endl <<
            "  add\t\tadd new backup job" << endl <<
            // "  edit\t\tedit backup job" << endl <<
            // "  remove\tremove backup job" << endl <<
            "  backup\trun backup job" << endl <<
            "  restore\trestore backup" << endl <<
            // "  rollback\trollback a file to older version" << endl <<
            "  diff\t\tshow difference between snapshots" << endl <<
            "  show\t\tshow files in specified snapshot" << endl <<
            "  history\tshow snapshots in a specified backup job" << endl <<
            // "  run-cron\trun planned backups" << endl <<
            "  help\t\tshow this help message" << endl << endl;

    cout << "Options" << endl;

    cout << "  -c\tspecify config directory" << endl <<
            "  -i\tspecify snapshot ID (to restore)" << endl <<
            "  -n\tspecify backup job name" << endl <<
            "  -s\tspecify new backup job source path" << endl <<
            "  -d\tspecify new backup job destination path" << endl <<
            "  -f\tspecify a path to a specific file (for restore)" << endl <<
            "  -x\tdisable compression (when adding a new backub job)" << endl;
    return 0;
}

string TerminalUserInterface::getVersion () {
    return "dev";
}

int TerminalUserInterface::add (char* source, char* destination, char* name, char* configPath, bool compress) {
    ConfigProvider* config = getConfigProvider(configPath);

    BackupJob* job = nullptr;
    try {
        std::string src = FilesystemUtils::AbsolutePath(source);
        std::string dst = FilesystemUtils::AbsolutePath(destination, true);
        if (FilesystemUtils::ArePathsEqual(src, dst))
            throw std::runtime_error("Source and destination leads to the same path.");
        job = new BackupJob(src, dst, name, compress);
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

int TerminalUserInterface::backup (char* name, char* configPath) {
    // add potential not enough space warning
    ConfigProvider* config = getConfigProvider(configPath);
    BackupJob* job;

    try {
        job = config->GetBackupJob(name);
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete config;
        return 2;
    }

    if (!job) {
        cerr << "Backup job named \"" << name << "\" not found." << endl;
        delete config;
        return 1;
    }

    try {
        job->Backup(this);
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

// todo better handle incorrect input (nonexistent snapshots/internal values)
int TerminalUserInterface::diff (char* name, int64_t snapshotIdA, int64_t snapshotIdB, char* configPath) {
    BackupIndexProvider* indexProvider = nullptr;
    ConfigProvider* config = getConfigProvider(configPath);
    BackupJob* job = nullptr;
    DirectoryIterator* itAdded = nullptr;
    DirectoryIterator* itRemoved = nullptr;
    DirectoryIterator* itModified = nullptr;

    try {
        job = config->GetBackupJob(name);
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete config;
        return 2;
    }

    if (!job) {
        cerr << "Backup job named \"" << name << "\" not found." << endl;
        delete config;
        return 1;
    }

    try {
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


        cout << "comparing snapshots " << snapshotIdA << " and " << snapshotIdB << endl;
        if (!itAdded->Empty()) {
            cout << format("files added", formatChars, true, TERM_GREEN) << endl;
            printTable(itAdded);
            cout << endl;
        }
        if (!itRemoved->Empty()) {
            cout << format("files removed", formatChars, true, TERM_RED) << endl;
            printTable(itRemoved);
            cout << endl;
        }

        TimeFileComparator comp;
        Directory modified = comp.CompareDirs(b, a) - added - removed;
        itModified = new DirectoryIterator(& modified);
        if (!itModified->Empty()) {
            cout << format("files modified", formatChars, true, TERM_BLUE) << endl;
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

int TerminalUserInterface::show (char* name, int64_t snapshotId, char* configPath) {
    BackupIndexProvider* indexProvider = nullptr;
    ConfigProvider* config = getConfigProvider(configPath);
    BackupJob* job = nullptr;
    DirectoryIterator* it = nullptr;

    try {
        job = config->GetBackupJob(name);
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete config;
        return 2;
    }

    if (!job) {
        cerr << "Backup job named \"" << name << "\" not found." << endl;
        delete config;
        return 1;
    }

    try {
        indexProvider = new SQLiteBackupIndexProvider(job);
        Directory files = indexProvider->LoadSnapshotFileIndex(snapshotId);
        it = new DirectoryIterator(& files);
        printTable(it);
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete job;
        delete indexProvider;
        delete config;
        delete it;
        return 2;
    }

    delete job;
    delete indexProvider;
    delete config;
    delete it;
    return 0;
}

int TerminalUserInterface::restore (char* name, int64_t snapshotId, char* filePath, char* configPath) {
    // todo refactor job lookup
    ConfigProvider* config = getConfigProvider(configPath);
    BackupJob* job;
    string file;

    try {
        job = config->GetBackupJob(name);
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete config;
        return 2;
    }

    if (!job) {
        cerr << "Backup job named \"" << name << "\" not found." << endl;
        delete config;
        return 1;
    }

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
                delete config;
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
                delete config;
                return 0;
            }

        } else {
            job->Restore(this, snapshotId, file);
        }

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

void TerminalUserInterface::UpdateProgress (size_t current, size_t expected, std::string status, size_t fileSize) {
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
void TerminalUserInterface::printTable (SimpleIterator* it) {
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

    while (!it->End()) {
        size_t i = 0;
        for (auto & val : it->TableRow()) {
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
    i = 0;

    // print data
    while (!it->End()) {
        for (auto & col : it->TableRow())
            cout << left << setw(colWidths[i++]) << col;
        i = 0;
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
