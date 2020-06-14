# backwither backup
Zálohovací progam s deduplikací a rychlou inkrementální zálohou, kompresí, 
možností navrátit se na libovolný stav souboru a plánovačem.

Návod k použití:
```
Commands
    list          show backup jobs
    add           add new backup job
    remove        remove backup job
    backup        run backup job
    restore       restore backup or a single file
    diff          show difference between snapshots
    show          show files in specified snapshot
    history       show snapshots in a specified backup job
    run           run all backups
    run-cron      run planned backups
    help          show this help message
Options
    -c    specify config directory
    -d    specify new backup job destination path
    -f    specify a path to a specific file (for restore)
    -i    specify snapshot ID (to restore)
    -m    disable filesystem time comparision speedup of backup
    -n    specify backup job name
    -p    specify a pair of snapshot to compare in diff [format id:id]
    -s    specify new backup job source path
    -t    specify a time to run a backup (when adding a new backup) [format 00:00-23:59]
    -w    specify a day of week to run a backup (when adding a new backup) [format mo-su]
    -x    disable compression (when adding a new backub)
```