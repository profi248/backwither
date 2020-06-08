#include <getopt.h>
#include <iostream>
#include <iomanip>
#include <filesystem>

#include "TerminalUserInterface.h"
#include "SQLiteConfigProvider.h"
#include "SQLiteBackupIndexProvider.h"
#include "FilesystemUtils.h"
#include "SnapshotListIterator.h"

using namespace std;

const int TABLE_FIELD_LENGTH = 20;

int TerminalUserInterface::StartInterface (int argc, char** argv) {
    // example from https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html#Using-Getopt

    opterr = 0;

    char* id          = nullptr;
    char* source      = nullptr;
    char* destination = nullptr;
    char* name        = nullptr;
    char* configPath  = nullptr;
    bool compress     = true;
    int  index;
    int  c;

    // default command is help
    if (argc == 1)
        help();

    // parse arguments: options need to be parsed first, then commands

    while ((c = getopt (argc, argv, "i:s:d:n:c:x")) != -1)
        switch (c) {
            case 'i':
                id = optarg;
                break;
            case 's':
                source = optarg;
                break;
            case 'd':
                destination = optarg;
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
                if (optopt == 'i' || optopt == 's' || optopt == 'd' || optopt == 'n' || optopt == 'c')
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
            // todo support specifing snapshot
            if (!name) {
                cerr << "Specifying backup name (-n) is required." << endl;
                return 1;
            }
            int64_t snapshotId = 0;
            if (id)
                snapshotId = strtoll(id, nullptr, 10);

            restore(name, snapshotId, configPath);
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
    // add last started column
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
            // "  diff\t\tshow difference between backups" << endl <<
            "  history\tpast backups history" << endl <<
            // "  run-cron\trun planned backups" << endl <<
            "  help\t\tshow this help message" << endl << endl;

    cout << "Options" << endl;

    cout << "  -c\tspecify config directory" << endl <<
            "  -i\tspecify snapshot ID to restore" << endl <<
            "  -n\tspecify new backup job name" << endl <<
            "  -s\tspecify new backup job source path" << endl <<
            "  -d\tspecify new backup job destination path" << endl <<
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

int TerminalUserInterface::restore (char* name, int64_t snapshotId, char* configPath) {
    // todo refactor job lookup
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
        cout << format("WARNING!", formattedChars) << " Snapshot you are restoring is not completed. Some files might not have been backed up." << endl;
    }

    try {
        if (!FilesystemUtils::IsDirectoryEmpty(job->GetSource())) {
            cout << format("WARNING!", formattedChars) << " Restore directory is not empty. All files in backup source ("
                 << job->GetSource()
                 << ") will be replaced by versions from the snapshot you selected! Continue? [y/N] "
                 << flush;

            if (yesNoPrompt()) {
                job->Restore(this, snapshotId);
            } else {
                delete job;
                delete config;
                return 0;
            }

        } else {
            job->Restore(this, snapshotId);
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
    else
        oss << "[" << current << "...] ";
    if (fileSize)
        oss << status << " (" << humanFileSize(fileSize) << ")";
    cerr << left << setw(m_LastStatusLength) << setfill(' ') << oss.str() << '\r' << flush;
    m_LastStatusLength = oss.str().length();

}

std::string TerminalUserInterface::humanFileSize (size_t bytes) {
    ostringstream oss;

    // bypass float if division not needed
    if (bytes < 1024) {
        oss << bytes << " B";
        return oss.str();
    }

    double size = bytes;
    int divisions = 0;
    while (size >= 1024) {
        size /= 1024;
        divisions++;
    }

    int precision = (divisions < 2) ? 2 : 3;

    oss << setprecision(precision) << size;
    switch (divisions) {
        case 1:
            oss << " KiB";
            break;
        case 2:
            oss << " MiB";
            break;
        case 3:
            oss << " GiB";
            break;
        case 4:
            oss << " TiB";
            break;
        case 5:
            oss << " PiB";
            break;
        case 6:
            oss << " EiB";
            break;
        default:
            throw bad_exception();
    }

    return oss.str();
}

std::string TerminalUserInterface::format (std::string in, int & formatChars, bool bold, int color) {
    if (bold) {
        formatChars = 8;
        return "\033[1m" + in + "\033[0m";
    }

    return in;
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
            cout << "Invalid answer. Please only type \"y\" for yes or \"n\" for no. [y/N] " << flush;
        }
    }

    return false;
}

void TerminalUserInterface::printTable (SimpleIterator* it) {
    if (!it)
        return;

    int colPadding = 2;

    vector<string> hdrs = it->TableHeader();
    vector<size_t> colHdrWidths;
    colHdrWidths.reserve(hdrs.size());
    vector<size_t> colWidths;


    if (it->TableRow().size() != hdrs.size())
        throw std::logic_error("Number of columns in header doesn't match number of coulumns in row.");

    for (auto & hdr : hdrs) {
        colHdrWidths.push_back(hdr.length() + colPadding);
    }

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

    size_t i = 0;
    int formatChars;
    for (auto & hdr : hdrs) {
        string formatted = format(hdr, formatChars);
        cout << left << setw(colWidths[i++] + formatChars) << formatted;
    }

    cout << endl;
    i = 0;

    while (!it->End()) {
        for (auto & col : it->TableRow())
            cout << left << setw(colWidths[i++]) << col;
        i = 0;
        cout << endl;
        (*it)++;
    }
}

