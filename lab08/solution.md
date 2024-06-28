# Iterace I.

### Úvodny prieskum

Po chvilke skúmania stránky sa dostaneme
na stránku [xmasdeal](http://boura.fit.cvut.cz/nightclub/xmasdeal.php),
čisto cez linky dosupné na stránke, tá obsahuje link na [download.php]()
s argumentom `file`.

Samozrejme sa pokusíme aku `file` argument zadať
`../../../../../../../../../proc/self/status`

```shell
curl "http://boura.fit.cvut.cz/nightclub/download.php?file=../../../../../../../../../proc/self/status"
```
A obdržíme odpoveď `This won't work!`.

Trochu sa teda uskromníme a vyskúšame súbor ktorý určite existuje a je 'bližšie': `file=index.php`, `file=../index.php`, `file=../../index.php`...

Hneď druhý pokus nám vráti source kód `index.php`.
Pomocou rovnakej techniky stiahneme všetky súbory ktorých meno trivialne vieme(názov súbory je na stránke uvedený).
Takto získame súbory: `bookgirl.php`, `db.php`, `download.php`, `footer.php`, `girldetail.php`, `header.php`,
`index.php`, `index-2.php`, `index-3.php`, `index-4.php`, `index-5.php`, `loginuser.php`, `middle.php`,
`newsfeed.php`, `style.css`, `xmasdeal.php`, pár obrázkov a `xmasdeal.pdf` ktorý sme mohli získať aj oficialne.

Ďalej by sme mohli urobiť útok cez zoznam častých názvov súborov
ako napríklad [tento](https://github.com/danielmiessler/SecLists/blob/master/Discovery/Web-Content/Common-PHP-Filenames.txt).
Urobené v [download_bruteforce.py](download_bruteforce.py) ale nezískalo to žiadne súbory navyše, okrem `favicon.ico`
ktorý nás nezaujíma.

Nemusíme skončiť pri názvoch súborov, ale môžeme získať aj názvy priečinkov.
Ak za file určíme `../DIR/../index.php`, tak pre DIR,
ktoré existujú dostaneme naspäť `index.php` ale pre neexistujúce nedostaneme nič pretože cesta nebude platná.
Skript: [dir_discovery.py](dir_discovery.py).
Nájdeme tak zložky: [`images`, `downloads`, `style`, `extra`, `sql`]

Nad `extra`, `sql` znovu spustíme [download_bruteforce.py]().
Nájdeme tak `sql/.htaccess` a `extra/.htaccess`.

Medzi súbormi je zaujímavý napríklad `db.php` kde vidíme ako je nastavená mysql:
- `hostname = null`, `port = null` znamená, že mysql použije `mysqli.default_host` a `mysqli.default_port`, ktorú nevieme
- `username = www_nightclub`, `password = iamamasterhacker`: vieme prihlasovacie údaje do DB.
- `socket = /var/run/mysqld/mysqld.sock`: vieme aký socket DB používa.

Ďalej v `download.php` vídíme kritickú chybu v `$requiredpath` keďže
zahŕňa celý priečinok `/var/www/html/nightclub`.
Následna kontrola potom povoluje sťahovať súbory aj zo zložky `nightclub`.

Ďalej `bookgirl.php` obsahuje SQL injection.
V zdrojovom kóde vidno neošetrený vstup `$_REQUEST['girl']`.

Ak by sme zdroje nemali k dispozícii museli by sme skúšať zadávať
do všetkých stránok neplatné vstupy a hľadať podozrivú odpoveď.
V tomto prípade by URL:
```shell
curl "http://boura.fit.cvut.cz/nightclub/bookgirl.php?girl='"
```
poskýtol indikáciu chyby v ošetrení vstupu.
Odpoveď: `Unable to query: You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near ''''' at line 1`

### Získavanie dát z DB

Jeden z problémov ktorý musíme vyriešiť je že `bookgirl.php` vypisuje vždy len jeden riadok výsledku.
Ako prvé si chceme vypísať `INFORMATION_SCHEMA.TABLES` aby sme vedeli aké
tabulky v DB sú.

Chviľku sa teda pohráme v online SQL editore a vymyslíme dotaz popísany v [query_1.md](queries/query_1.md).

Teraz vieme názvy tabuliek, potrebujeme ešte zistiť názvy stĺpcov v tabulkách.
Vytvoríme teda [query_2.md](queries/query_2.md).

Keď vieme názvy stĺpcov aj tabuliek, môžeme vytiahnúť všetky data z tabuliek.
Postup v [query_3.md](queries/query_3.md).

Zístíme tak login/heslo na `root:Tr0ub4dor&3` a `www:nemesis`.


### Oprava chýb v aplikácii

Je potrebné opraviť SQL injection pridaním `db->real_escape_string` pre užívatelský vstup: [fixed_bookgirl.php](app_download/fixed_bookgirl.php)

Tiež je nutné zabrániť sťahovaniu súborov mimo `downloads` zložku, upraceným `$requiredpath`: [fixed_download.php](app_download/fixed_download.php)

Tiež by bolo pekné pridať salt k hashovaným heslam.
Implementáciu nemám, bolo by nutné pridať stĺpec `salt` do tabulky `accounts`.


### Extra

Ďalšie pokusy dozvedieť sa niečo viac o DB aj keď k ničomu významnému neviedli:

Môžeme vypísať USER_PRIVILEGES z INFORMATION_SCHEMA:
- grantee: 'www_nightclub'@'localhost'
- type: USAGE
- is_grantable: NO

Časť dotazu:
```mysql
SELECT JSON_ARRAYAGG(CONCAT_WS(',',
COALESCE(GRANTEE,''),COALESCE(PRIVILEGE_TYPE,''),COALESCE(IS_GRANTABLE,'')
)),NULL,NULL,NULL FROM INFORMATION_SCHEMA.USER_PRIVILEGES
```

K tabuľke INFORMATION_SCHEMA.FILES nemáme prístup:
`Unable to query: Access denied; you need (at least one of) the PROCESS privilege(s) for this operation`

Tabuľky INFORMATION_SCHEMA.TABLE_PRIVILEGES sa zdá byť prázdna.

Tabuľky ADMINISTRABLE_ROLE_AUTHORIZATIONS, USER_ATTRIBUTES, ENABLED_ROLES, APPLICABLE_ROLES
asi nie sú vo verzii mysql na serveri podporované:
`Unable to query: Unknown table 'ENABLED_ROLES' in information_schema`
