# Záklane riešenie

- Používateľ pri počítači je schopný vyžiadať čas v ľubovolnom 
formáte pomocou príkazu `get:<format>\n`.

- Používateľ pri počítači je schopný nastaviť čas pomocou
príkazu `set:<format>\n<formatted time>\n`

- Sieťový užívateľ obdrží správu `"Low privilege for set command."`
pri pokuse o nastavenie času a čas sa mu zmeniť nepodarí.

- Aplikace je rozdelená na programy `net_clock` a `change_time`:
  - Základna časť aplikace(`net_clock`) beží pod bežným užívatelom bez žiadných capabilities.

  - Časť aplikace ktorá mení čas je v `change_time` a tiež beží
  pod bežným uživatelom ale s nastavenou capability `cap_sys_time`
  aby bola schopná meniť systémový čas.

  - Aplikace `net_clock` spúšťa `change_time` aby nastavila čas,
    `change_time` súbor ma nastavený `cap_sys_time` capability.

  - Program `change_time` je veľmi jednoduchý a okrem kontroly vstupu
  volá len funkce `fprintf`, `strtoll`, `clock_settime`, `perror`.

  - Program `change_time` neiteraguje s užívatelem.

# Úplne riešenie

- Aplikace `net_clock` aj `change_time` beží pod bežným užívatelom
takže nemajú žiadne privilegia. Jedine `change_time` je už spustený
s capability `cap_sys_time`.

- Robím svoje riešenie na Ubuntu 22, a DEP je defaultne zapnutý `https://wiki.ubuntu.com/Security/Features#nx`

- Aplikace umožnuje uživátelovy zadať ľubovolný reťazec podporovaný funkciou `strftime`.

- Počas jedného spojenia môže užívateľ zadať viac požiadavkov a
spojenie je ukončené až užívatelom alebo chybou pri vyhodnocovaní požiadavku.

- Aplikací je možné konfigurovať pomocou konfiguračného súboru.
Konfiguračný súbor môže byť umiestnený v:
(`$HOME/.config/net_clock.conf`,
 `$HOME/net_clock.conf`,
 `$HOME/.net_clock.conf`,
 `/etc/net_clock.conf`,
 cesta v env premennej `"$NET_CLOCK_CONF"`)

  Prvý nájdený súbor je použitý.
  Prvé tri umiestnenia umožnujú užívatelovy robiť svoje nastavenia.
  Umiestnenie `/etc/net_clock.conf` je zapisovatelne len `root` uživatelom
  a umožnuje meniť nastavenie napríklad administrátorovy meniť predvolené nastavenia.
  
  Cesta určená env premennej môže byť užitočná pri použitý s inými nastrojmi(nasadzovanie na docker).


# Pokus II.

- Oba programy `change_time` a `net_clock` sa zbavia na
začiatku všetkých nepotrebných capabilities. `net_clock` sa zbavý
všetkých capabilities. `net_clock` si ponechá jedine `cap_sys_time`
v effective a permitted množine aby bol schopný zmeniť čas.

- Vypustené chybové hlášky z `change_time.c`.

- Pridal som možnosť normálnej inštalácie:

  - inštalácia možná pomocou `make install`

  - súbory sú vlastnené `root:root` aby ich nemohol obyčajný užívateľ meniť/mazať

  - programy sú umiestnené v zložke `/usr/local/bin`, ktorá je určená pre aplikácie pre užívatela.

  - `change_time` je spúštaný cez absulutnú cestu v `/usr/local/bin`

  - odinštalovanie možné pomocou `make uninstall`

- Prepísal som `format` aby neprímala formáty ktoré nie sú popísané v dokumentácii.
Rozdelenie časti funkcionality `format` do `get_conversion_spec`. Pridaná lepšia dokumentácia do implementácie `net_clock.c:format`.

# Pokus III.

```
> Bezpečnostní výhradu mám k spouštění procesu pro nastavení času. V jakém
> adresáři podle vás tento soubor leží? Zdůvodněte, proč to považujete za
> dobrý nápad. (Zaměřte se hlavně na první otázku, podle mě jste měl v
> úmyslu něco jiného, než jste do programu skutečně napsal.) [-2 b.]

Nové řešení jde přijmout, i když je uživatelsky poněkud nepřátelské. Ta
původní myšlenka mi přišla lepší, akorát jste ji měl správně
naimplementovat.
```

Moje predchádzajúce riešenie vôbec neriešilo prípad kedy
je `net_clock` spustený mimo zložky kde je umiestnený.
Mohol som to vyriešiť nájdením umiestnenia `net_clock`
za behu z `/proc/self/exe` a predpokladať že `change_time`
sa vyskytuje v rovnakej zložke. To mi neprišlo ako
pekné riešenie keďže to ponecháva spustiteľné súbory
na mieste kde si užívatel stiahol zdrojáky a zbuildil ich.
To môže byť napríklad aj v `~/Downloads`.

Umiestnenie v `/bin`, `/usr/bin`, `/usr/local/bin` zaručuje
že užívatel nemá prístup k samotnej zložke v ktorej je súbor umiestnený.

```
> Funkce format() mi přijde hrozně komplikovaná. Můžete prosím vysvětlit
> její úvodní část (před voláním strftime)?

Myslíte, že je to bezpečnostně přínosné nebo ne?
```

V prvej verzii aplikace som parsovanie formatovácieho 
znaku vôbec nekontroloval. Mohlo sa stať že užívatel poslal '%*',
čo nie je validny formátovací reťazec, ktorý som následne poslal do strftime.
strftime má nedefinované chovanie pri nedefinovaných formátovácich
reťazcoch.

Taktiež mojá prechádzajúca verzia nebola schopná
riešiť rozšírené formátovacie flagy '_-0^#', takže vo výsledku
aj správne formátovacie reťazve (v glibc rezšírení)
boli nesprávne parsované.

Aj napriek týmto chybam aplikácia správne pracovala s dĺžkami
reťazcov/bufferov takže bezpečnostné riziko sa presúva
z funkcie format do funkcie strftime.

Keďže, ako som už zmienil, strftime má nedefinované chovanie
pre neznáme formátovacie reťazce môže sa jednať o bezpečnostné riziko.
Zároveň by ma však prekvapilo ak by nejaká implementácia
strftime nebola schopná bezpečne zvládnuť aj nedefinované
formátovacie reťazce a spôsobovala chybu ktorá sa dá
využiť útočnikom (buffer overflow, výpis stacku, ...).

Poznamka na koniec:

Na internete som našiel rôzne implementácie ktoré
riešia prípad neplatného formátu rozlične.

"https://github.com/arnoldrobbins/strftime/blob/master/strftime.c":
Skopíruje do výsledku '%' aj neplatný formátovací znak.

"https://android.googlesource.com/platform/bionic/+/master/libc/tzcode/strftime.c":
Skopíruje do výsledku len samotný neplatný formátovaci znak bez '%'.

