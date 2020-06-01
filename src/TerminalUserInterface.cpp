#include <getopt.h>
#include <iostream>
#include <iomanip>
#include <exception>

#include "TerminalUserInterface.h"
#include "SQLiteConfigProvider.h"
#include "BackupPlanIterator.h"
#include "BackupJob.h"

using namespace std;

const int TABLE_FIELD_LENGTH = 20;

int TerminalUserInterface::StartInterface (int argc, char** argv) {
    // example from https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html#Using-Getopt

    opterr = 0;

    char *source      = nullptr;
    char *destination = nullptr;
    char *name        = nullptr;
    char *configPath  = nullptr;
    int  index;
    int  c;

    // default command is help
    if (argc == 1)
        help();

    // parse arguments: options need to be parsed first, then commands

    while ((c = getopt (argc, argv, "s:d:n:c:")) != -1)
        switch (c) {
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
            case '?':
                if (optopt == 's' || optopt == 'd' || optopt == 'n' || optopt == 'c')
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
        } else if (command == "add") {
            if (!source || !destination || !name) {
                cerr << "Specifying backup source path (-s), destination path (-d) and name (-n) is required." << endl;
                return 1;
            }

            add(source, destination, name, configPath);
        } else if (command == "run") {
            if (!name) {
                cerr << "Specifying backup name (-n) is required." << endl;
                return 1;
            }

            run(name, configPath);
        } else if (command == "restore") {
            // todo support specifing snapshot
            if (!name) {
                cerr << "Specifying backup name (-n) is required." << endl;
                return 1;
            }

            restore(name, configPath);
        } else {
            cerr << "Command " << command << " not recognized." << endl;
            return 1;
        }
    }

    return 0;
}

int TerminalUserInterface::list (char* configPath) {
    ConfigProvider* config = getConfigProvider(configPath);

    BackupPlan* plan;
    try {
        plan = config->LoadBackupPlan();
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
        delete config;
        return 2;
    }
    BackupPlanIterator it(plan);

    if (it.Empty()) {
        cout << "No backup jobs. Add a new job by running `add`." << endl
             << "If you expect to see something, please verify that correct config is in place." << endl;

        delete plan;
        delete config;
        return 0;
    } else {
        cout << "Backup jobs:" << endl;
    }

    cout << left << setw(TABLE_FIELD_LENGTH) << "name"
         << setw(TABLE_FIELD_LENGTH) << "source" << setw(TABLE_FIELD_LENGTH) <<
            "destination" << setw(TABLE_FIELD_LENGTH) << "incremental" << endl;

    // todo improve table... (setw should be longest displayed value)
    // todo refactor into table function
    while (!it.End()) {
        cout << left << setw(TABLE_FIELD_LENGTH) << it.GetName()
                     << setw(TABLE_FIELD_LENGTH) << it.GetSource()
                     << setw(TABLE_FIELD_LENGTH) << it.GetDestination()
                     << setw(TABLE_FIELD_LENGTH) << boolalpha << it.GetIncremental() << endl;
        it++;
    }

    delete plan;
    delete config;
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
            "  edit\t\tedit backup job" << endl <<
            "  remove\tremove backup job" << endl <<
            "  backup\trun backup job" << endl <<
            "  restore\trestore backup" << endl <<
            "  rollback\trollback a file to older version" << endl <<
            "  diff\t\tshow difference between backups" << endl <<
            "  history\tpast backups history" << endl <<
            "  run-cron\trun planned backups" << endl << endl;

    cout << "Options" << endl;

    cout << "  -c\tspecify config directory" << endl <<
            "  -n\tspecify new backup job name" << endl <<
            "  -s\tspecify new backup job source path" << endl <<
            "  -d\tspecify new backup job destination path" << endl;
    return 0;
}

string TerminalUserInterface::getVersion () {
    return "dev";
}

int TerminalUserInterface::add (char* source, char* destination, char* name, char* configPath) {
    ConfigProvider* config = getConfigProvider(configPath);

    // todo support non-incremental
    BackupJob* job = new BackupJob(source, destination, name, true);

    try {
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

int TerminalUserInterface::run (char* name, char* configPath) {
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
        job->Backup(config, this);
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

int TerminalUserInterface::restore (char* name, char* configPath) {
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

    // todo check if dir is empty
    string answer;
    cout << "WARNING! ALL files in backup source (" << job->GetSource() << ") will be replaced by versions from the snapshot you selected!"
            " Continue? [N/y] " << flush;
    while (answer != "n" && answer != "y" && answer != "N" && answer != "Y") {
        getline(cin, answer);
        if (answer == "n" || answer == "N" || answer.empty()) {
            delete job;
            delete config;
            return 0;
        } else if (answer == "y" || answer == "Y") {
            try {
                job->Restore(config, this, 0); // todo pass snapshot id
            } catch (runtime_error & e) {
                cerr << "Fatal error: " << e.what() << endl;
                delete job;
                delete config;
                return 2;
            }
        } else {
            cout << "Invalid answer. Do you want to replace all files? [N/y] " << flush;
        }
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

    oss << status << " (" << humanFileSize(fileSize) << ")";
    cout << left << setw(m_LastStatusLength) << setfill(' ') << oss.str() << '\r' << flush;
    // cout << m_LastStatusLength;
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
