#include <getopt.h>
#include <iostream>
#include <exception>

#include "TerminalUserInterface.h"
#include "SQLiteConfigProvider.h"
#include "BackupPlanIterator.h"

using namespace std;

int TerminalUserInterface::StartInterface (int argc, char** argv) {
    // example from https://www.gnu.org/software/libc/manual/html_node/Using-Getopt.html#Using-Getopt

    opterr = 0;

    char *path       = nullptr;
    char *name       = nullptr;
    char *configPath = nullptr;
    int  index;
    int  c;

    // default command is help
    if (argc == 1)
        help();

    // parse arguments: options need to be parsed first, then commands

    while ((c = getopt (argc, argv, "p:n:c:")) != -1)
        switch (c) {
            case 'p':
                path = optarg;
                break;
            case 'n':
                name = optarg;
                break;
            case 'c':
                configPath = optarg;
                break;
            case '?':
                if (optopt == 'p' || optopt == 'n' || optopt == 'c')
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
            list(configPath);
        } else if (command == "add") {
            if (!path || !name) {
                cerr << "Specifying backup path (-p) and name (-n) is required." << endl;
                return 1;
            }

            add(path, name);
        } else {
            cerr << "Command " << command << " not recognized." << endl;
            return 1;
        }
    }

    return 0;
}

int TerminalUserInterface::list (char* configPath) {
    ConfigProvider* config;
    if (configPath)
        config = new SQLiteConfigProvider(configPath);
    else
        config = new SQLiteConfigProvider();

    cout << "Backup jobs:" << endl;
    BackupPlan plan;
    try {
        plan = config->LoadBackupPlan();
    } catch (runtime_error & e) {
        cerr << "Fatal error: " << e.what() << endl;
    }
    BackupPlanIterator it(plan);

    while (!it.End()) {
        // todo actually output
    }

    delete config;
    return 0;
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
            "  -p\tspecify new backup job path" << endl;
    return 0;
}

string TerminalUserInterface::getVersion () {
    return "dev";
}

bool TerminalUserInterface::add (char* path, char* name) {
    // todo
    return false;
}
