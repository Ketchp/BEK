# Ukol #1

Výpis z my_gets:

```
(gdb) x/120gx $rsp
0x7fffffffe300:	0x0000000000000007  <- rsp, lokalna premenná(buff_size)
0x7fffffffe308:	0x00007fffffffe330  <- adresa na buffer, lokalna premenná(buffer)
0x7fffffffe310:	0x0a007fffffffe350
0x7fffffffe318:	0x0000006600000006  <- lokalne premenne `c` a `chars_read`
0x7fffffffe320:	0x00007fffffffe3b0  <- rbp, uložená rbp pre main
0x7fffffffe328:	0x0000555555555d2e  <- návratová adresa
0x7fffffffe330:	0x0000666564636261  <- buffer(start) = (char (*)[32])
0x7fffffffe338:	0x00005555555559ac
0x7fffffffe340:	0x0000555555558d30
0x7fffffffe348:	0x0000555555555a88  <- buffer(end)
0x7fffffffe350:	0x00000006550a4040
0x7fffffffe358:	0x00007fff00000001
0x7fffffffe360:	0x0000000000000020	0x00007fffffffe819
0x7fffffffe370:	0x00007ffff7fc1000	0x000000000000001f
0x7fffffffe380:	0x00007fffffffe330	0x00000000bfebfbff
0x7fffffffe390:	0x00007fffffffe829
0x7fffffffe390:	0x42a9e00646b10900  <- canary
0x7fffffffe3a0:	0x0000000000000000	0x00007fffffffe4c8
0x7fffffffe3b0:	0x0000000000000001	0x00007ffff7c29d90
0x7fffffffe3c0:	0x0000000000000000	0x00005555555559ac
0x7fffffffe3d0:	0x00000001ffffe4b0	0x00007fffffffe4c8
0x7fffffffe3e0:	0x0000000000000000	0x3bd39e7a44a3a1c6
0x7fffffffe3f0:	0x00007fffffffe4c8	0x00005555555559ac
0x7fffffffe400:	0x0000555555558d30	0x00007ffff7ffd040
0x7fffffffe410:	0xc42c61858321a1c6	0xc42c71ff7e29a1c6
0x7fffffffe420:	0x00007fff00000000	0x0000000000000000
0x7fffffffe430:	0x0000000000000000	0x0000000000000000
0x7fffffffe440:	0x0000000000000000
0x7fffffffe440:	0x42a9e00646b10900  <- another canary
0x7fffffffe450:	0x0000000000000000	0x00007ffff7c29e40
0x7fffffffe460:	0x00007fffffffe4d8	0x0000555555558d30
```

Pre úplnosť aj bod v ktorom bol stack vypísaný:
```
(gdb) info stack 
#0  my_gets (buffer=0x7fffffffe330 "abcdef", buff_size=7) at main.c:36
#1  0x0000555555555d2e in main () at main.c:212
#2  0x00007ffff7c29d90 in __libc_start_call_main (main=main@entry=0x5555555559ac <main>, argc=argc@entry=1, argv=argv@entry=0x7fffffffe4c8) at ../sysdeps/nptl/libc_start_call_main.h:58
#3  0x00007ffff7c29e40 in __libc_start_main_impl (main=0x5555555559ac <main>, argc=1, argv=0x7fffffffe4c8, init=<optimized out>, fini=<optimized out>, rtld_fini=<optimized out>, stack_end=0x7fffffffe4b8) at ../csu/libc-start.c:392
#4  0x00005555555552c5 in _start ()
```

# Ukol #2
Útok bude pozostávať z dvoch časti:

### 1. Najdenie kanárka
Využijeme chybu v implementácii `game`, ktorá spôsobuje
to že môžeme čítať pamäť(až `255B`) za bufferom `word`.

Postupne budeme posielať reťazce dlhé `256B` pozostávajúce z
rovnakého bytu a ukončené `\n`.
(`0x0 0x0 0x0 ... 0x0 \n`, `0x1 0x1 ... \n`, `0x2 0x2 ...`, `...`)
Na základe odpovede hry ktorá porovná náš retazec s bufferom a pamätou 
za ním budeme schopný zistiť kde všade v pamäti sa nachádza konkrétna hodnota bytu.
Vyskušaním všetkých `256` bytov vieme zmapovať celú pamäť.
(`\n` zmapovať nevieme ale to bude zvyšok pamäte kde sa nenachádzal žiadny iný byte)
Táto pamäť obsahuje kanarka, adresu na stack a adresu na main(potrebné pre obídenie ASLR).

V tento moment môžeme ukončiť hádanie a zadať spravny reťazec aby nas hra pustila
do dalšieho kola.

### 2. Pretečenie zásobniku
V druhej fáze utoku pošleme do programu konštantu `INT32_MIN`,
ktorá spôsoby pretečenie premennej `word_len` a umožní nám
do bufferu zapisovať skoro neobmedzene množstvo znakov.

Na základe informácii z prvej fázy útoku budeme schopný vytvoriť
pretečenie tak aby návratová adresa ukazovala do stredu `safe_memcpy`,
a aby parametre tejto funkcie upravili retazec `cmd_sleep` na nami požadovaný program.

Ďalšiu návratovú adresu nastavíme na stred funkcie `func_1`,
tesne pred načítanim argumentov do registrov a zavolaním `execve`.

Po vložení potrebných znakov ukončíme vstup pomocou `\n` čo ukončí načitanie.
Všimnime si že preteženie nenastalo v `my_gets` ale vo funkci `main`, takže
`my_gets` sa ešte bezchybne vráti do `main`.
Tam hra porovná počet načítaných znakov s očakávaním počtom znakov.
Keďže počet znakov bude rozdielny začne sa dalšie kolo.

Následne zadáme `0\n`, aby sme ukončili hru.
Potom sa vykoná `return 0;` z `main` čo spustí náš útok.

Poznámka 1: Volaniu `safe_memcpy` sme sa vedeli úplne vyhnuť a vytvoriť
všetky potrebné štrukturý pre `execve` na pretečenom zásobniku.

Poznámka 2: `func_1` je veľka pomoc pre return oriented programing,
keďže argumenty `execve` sú argumentami aj `func_1`, čo umožnuje jednoduché naplnenie
potrebných registrov. Iný spôsob pre naplnenie registov som bohužial nenašiel,
aj keď som si celkom istý že nejakým spôsobom by to išlo.

Poznámka 3: Tento útok je prakticky nemožne výkonať ručne z terminálu keďže
len prvá fáza útoku výžaduje zapísanie `256 * 256 ~ 65000` znakov
a následne spracovanie odpovede hry pre zistenie obsahu pamäte.

Poznámka 3.1: Ak by sme neboli schopny zadávať do programu
všetky hodnoty bytu od 0 do 255, utok by stalé bolo
možne z časti previesť. V prvej časti by sme zmapovali hodnoty
ktoré zmapovať vieme, čo by mohlo znížiť
počet možných hodnôt kanarká a adries dostatočne na to aby
sme vedeli skúšaním uhádnuť hodnoty kanárka/adries.
(Vyžadovalo by to však opakované spúštanie programu a
prípadné spadnutia.) V druhej faze by bola nedostupnosť
niektorých znakov väčší problem, ale tiež by sme si
vedeli pomôcť tým že navratová adresa niekedy môže ukazovať
aj skôr do funkcie a `rbp` môžeme naopak voliť výššie.

Poznámka 3.2: Analýza hodnôt ktoré by bolo možné zadať pri útoku
zápisom do terminálu:

```
Dec | Hex | Ako
  0 |   0 | CTRL + "V@"
  1 |   1 | CTRL + "A"
  2 |   2 | CTRL + "B"
  3 |   3 | CTRL + "VC"
  4 |   4 | CTRL + "VD"
  5 |   5 | CTRL + "E"
  6 |   6 | CTRL + "F"
  7 |   7 | CTRL + "G"
  8 |   8 | CTRL + "H"
  9 |   9 | CTRL + "I"
 10 |   a | možné zadať ale čítanie je my_gets ukončuje čítanie
 11 |   b | CTRL + "K"
 12 |   c | CTRL + "L"
 13 |   d | CTRL + "VM"
 14 |   e | CTRL + "N"
 15 |   f | CTRL + "O"
 16 |  10 | CTRL + "P"
 17 |  11 | CTRL + "VQ"
 18 |  12 | CTRL + "VR"
 19 |  13 | CTRL + "VS"
 20 |  14 | CTRL + "T"
 21 |  15 | CTRL + "VU"
 22 |  16 | CTRL + "VV"
 23 |  17 | CTRL + "VW"
 24 |  18 | CTRL + "X"
 25 |  19 | CTRL + "Y"
 26 |  1a | CTRL + "VZ"
 27 |  1b | CTRL + "["
 28 |  1c | CTRL + "V\"
 29 |  1d | CTRL + "]"
 30 |  1e | CTRL + "^"
 31 |  1f | CTRL + SHIFT + "_"
 32 |  20 | bežné znaky
            ...
126 |  7e | bežné znaky
127 |  7f | CTRL + "V", backspace  alebo  CTRL + "V?"
128+|  80+| pri UTF-8 znakoch nemožné, niektoré hodnoty
          | by bolo možné zadať pomocou UNICODE znakov
          | ktoré by ako prvý byte obsahovali byte
          | z rozsahu <128,255> ale následovali by další byty
```

# Ukol #3
Úprimne neviem.

# Ukol #4
Kód exploitu nie je možné staticky určiť,
keďže jeho druhá časť (pretečenie) závisi na
odpovedí hry z prvej časti ktorá obsahuje kanárka
a adresy na stack/main.

Upravil som `attack.py` tak aby duplikoval 
vstup/výstup z programu takže ukážkovy
vstup je v súbore `stdin_copy.bin` (`~65kB`).

Detailny popis útoku byte po byte je
v samotnom kóde `attack.py`.

Poznamka: Pre záchyt vstupu/vystupu útoku je potrebné
spustit útok s flagom:

```shell
python3 attack.py --capture
```
