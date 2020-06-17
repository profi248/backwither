# backwither backup
Zálohovací program s deduplikací a rychlou inkrementální zálohou, kompresí,
možností navrátit se na libovolný stav souboru a plánovačem.

Program si udržuje seznam záloh. Pokud chceme zálohovat, začneme přidáním zálohy příkazem `add`
se jménem, kterým se bude později spouštět. Veškeré parametry se předávají pomocí přepínačů v argumentech.
Seznam všech možností naleznete níže. Jakmile se záloha přidá,
je možné si zobrazit seznam záloh příkazem `list`, nebo zálohu rovnou spusit 
příkazem `backup`. Každým spuštěním zálohy se vytvoří nový snapshot, do kterých se dá libovolně vracet.
Zobrazíme je příkazem `history.` Poté, co záloha proběhla je možné ji celou obnovit (z destinace do zdroje)
příkazem `restore`. Stejným příkazem můžeme obnovit i samotný soubor, pokud jeho cestu relativní vzhledem
k zálohované složce předáme přepínači `-f`. Také můžeme specifikovat ID snapshotu přepínačem `-i`.

Porovnávat rozdíly mezi snapshoty je možné pomocí příkazu diff, který vyžaduje pár snapshotů k porovnání.
Předá se pomocí parametru `-p` a specifikuje se ve formátu id1:id2. Tedy například `-p 12:14`.

Podporovány jsou i plánované zálohy. Týdenní plán se nastaví při přidávání zálohy. Poté se do systémového
plánovače přidá příkaz `run-cron` (například v intervalu každou hodinu), který zkontroluje, zda není čas
spustit nějakou zálohu (příkaz nemusí běžet přesně v minutu kdy je záloha naplánována, spustí se všechny
zálohy, co od posledního naplánovaného data nebyly spuštěny).

Program ukládá zálohovaná data jako bloky vziklé rozsekáním zálohovaných souborů
(v současnosti po fixních částech o velikosti 1 MiB), což umožňuje deduplikaci (ukládání dat pouze jednou)
nejen u jednoho souboru, ale také napříč celou zálohou a to na úrovni částí souboru. Díky tomu se
ušetří mnoho místa, když se například na konec velkého souboru přidají data.
Není potřeba zálohovat celý soubor znovu.

Tato deduplikace funguje na bázi hashování jednotlivých chunků a v případě že je chunk s určitým hashem
již uložen, není potřeba ho ukládat znovu. Tady by čistě teoreticky mohlo dojít ke kolizi hashů, ale
backwither využívá hashovací algoritmus SHA-256, který má celkem 2^256 kombinací.
Mizivé šance kolize 10^-18 dosáhneme (při velikosti chunku 1 MiB)
až po zálohování stovek miliard exabytů (1). Dříve než toto může nastat,
dosáhneme na mnoho různých softwarových i hardwarových limitů.

Pokud není zakázána komprese, každý chunk je také komprimován knihovnou zlib (Deflate algoritmus).
Program ignoruje kompletně prázné složky, podobně jako Git.

Seznam záloh (SQLite databáze) je ve výchozím nastavení ukládán do domovské složky uživatele. 
Index souborů, chunků a snapshotů v záloze je uložen také v SQLite databázi v destinaci zálohy
společně se samotnými daty. 

(1) tabulka pravděpodobnosti kolizí: https://en.wikipedia.org/wiki/Birthday_problem#Probability_table

### Rozhraní příkazové řádky:

```
Commands
  list          show backup jobs
  add           add new backup job (optionally with schedule)
  remove        remove backup job
  backup        run backup job
  restore       restore backup or a single file (optionally specify snapshot with -i)
  diff          show difference between snapshots (specify snapshots to compare with -p)
  show          show files in specified snapshot
  history       show snapshots in a specified backup job
  run-all       run all backups
  run-cron      run planned backups due for a run
  help          show this help message

Options
  -c    specify config directory
  -d    specify new backup job destination path
  -f    specify a path to a specific file (for restore or diff)
  -i    specify snapshot ID (to restore)
  -m    disable filesystem time comparison speedup of backup
  -n    specify backup job name
  -p    specify a pair of snapshot to compare in diff [format id:id]
  -s    specify new backup job source path
  -t    specify a time to run a backup (when adding a new backup) [format 00:00-23:59]
  -w    specify a day of week to run a backup (when adding a new backup) [format mo-su]
  -x    disable compression (when adding a new backup)
```


### Využití polymorfismu 

Polymorfismus je využíván například v iterátorech, které dědí ze třídy SimpleIterator.
Tyto iterátory implementují obyčejné metody z iterátrů, jako posuv. Také implementují
metody pro zobrazení tabulky s formátovanými daty (hlavička tabulky a řádky).
Díky tomu může metoda printTable zobrazit tabulky s růzými daty v jedné funkci,
která využívá unifikované rozhraní iterátorů.   

Další polymorfická třída je ChunkStorageProvider, ze které vychází 
FilesystemChunkStorageProvider, která ukláda data na lokální disk
a CompressedFilesystemChunkStorageProvider, která je předtím ještě komprimuje (a dekomprimuje).

Mezi další polymorfické třídy patří BackupJob ze které vychází 
TimedBackupJob a přidává časové údaje a funkce k regulérní uložené záloze.
