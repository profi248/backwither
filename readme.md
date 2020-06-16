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

Seznam záloh (SQLite databáze) je ve výchozím nastavení ukládán do domovské složky uživatele. 
Index souborů, chunků a snapshotů v záloze je uložen také v SQLite databázi v destinaci zálohy
společně se samotnými daty. 

(1) tabulka pravděpodobnosti kolizí: https://en.wikipedia.org/wiki/Birthday_problem#Probability_table

### Návod k použití:
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


[TODO update] Využití polymorfismu 
--------------------

Polymorfismus používám u třídy `DirectoryComparator`, která je implementovaná třídami `HashFileComparator` a `TimeDirectoryComparator`,
které využívají různé metody pro porovnávání, zda se soubory pozměnily. Jedna metoda je pomocí hashů (například md5).
Druhá metoda je pomocí časů z filesystému. Ta nemusí být vždy 100% přesná, ale je rychleší. Naopak hashe reprezentují reálný
obsah souborů, a o výsledku tedy není pochyb. Proto se hodí implementovat obě varianty.

Další příklad polymorfismu je třída `FilesytemEntity`, ze které se dědí třídy `File`, `Directory` a `Link`.
Tyto třídy se používají na prácí s filesystémem. Složka drží seznam podřazených filesystém entit a 
poskytuje funkce pro práci s nimi, soubor poskytuje funkce pro práci s porovnáváním a kopírováním 
souborů a odkaz obsahuje referenci na filesystém entitu, na kterou odkazuje.
Tento polymorfismus ale možná nebude ve finální podobě programu potřeba a proto bude odstraněn.

Dále je použit u třídy `ChunkStorageProvider`, ze které se dědí implementace úložiště
(například lokání filesystém), ale mohl by být i pro sftp apod.
Podobným případem je třída `ConfigProvider`, která má na starosti uložení seznamu záloh a jejich konfigurací,
a je implementovaná například třídou `SQLiteConfigProvider` jako provider, co ukládá do SQLite databáze.
Opět podobná je třída `TerminalUserInterface`, která implementuje `UserInterface` jakož terminál. Šla by doplnit například
třídou `NCursesUserInterface`, která využívá ncurses rozhraní.


Další třída, která by dávala smysl implementovat polymorfismem je `TimedBackupJob`, která vychází z 
`BackupJob` a přidává časové údaje a funkce k regulérní uložené záloze.