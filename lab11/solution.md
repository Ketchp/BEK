# Solution

V [shot.html](injection/shots.html) je druhý skript schopný zmeniť posledný príspevok na stránke na `Hacked`.

Tretí skript je ešte navýše schopný sam sa zmazať po tom ako prevedie útok takže ak by sa obeť pozrela na DOM nenšla by náš útok. Stále je samozrejme možné nájsť náš skript ak sa pozrieme na page source(Ctrl+U).

Štvrtý skript som pridal aby som zabranil redirectu na PH. Skript urobí alert, pred tým ako sa načíta skript ktorý robí redirect. To dá užívatelový čas aby otvoril developer-tools a niečo napísal do `console`, to zapríčiní že Chrome(na ostatných prehliadačoch som to netestoval ale malo by to fungovať aj inde) bude považovať stránku za `sticky activated` (https://developer.mozilla.org/en-US/docs/Glossary/Sticky_activation). Potom skript nastaví callback na `beforeunload` event ktorý je volaný pred zatvorením stránky (teda aj pri redirecte). Callback spôsobí to že upozorní užívatela o odchode zo stránky a dá mu možnosť zrušiť odchod zo stránky ale len ak prebehla `sticky activation` (https://developer.mozilla.org/en-US/docs/Web/API/Window/beforeunload_event#usage_notes).

Ďalej vieme vytvoriť príspevok, ktorý pridá príspevok na náš profil. Spustíme developer-tools a odchýtime request ktorý sa pošle pri pridaní príspevku a vykopírujeme request ako cURL príkaz. cURL príkaz je v súbore [new_post.sh](injection/new_post.sh). V súbore vieme priamo meniť obsah/text pridaného príspevku a pokiaľ vieme cookie iného uživateľa tak vieme pridať príspevok v mene iného užívateľa.

Piaty skript zbiera cookies užívatelov a posiela ich na oracle VM server, source serveru v zložke [server](server). Domain serveru je podobná adrese samotnej stránky boura.cvut.fit.cz vs boura.cvut-fit.cz. Skript sa opäť sám vymaže z DOM aby ztažil odhalenie.

Pre pridanie príspevku za iného užívateľa stačí pozmeniť len cookie header v [new_post.sh](), za ukradnutý cookie. Netreba dokonca meniť ani `profile_user_id` v `--data-raw` alebo `Referer` header kde je tiež id užívateľa.

Pre pridanie užívateľa do skupiny povolíme `Preserve log` `developer-tools: Network` a odchytíme request pre pridanie nášho užívatela. Výsledny request je vykopírovaný do [page_like.sh](injection/page_like.sh). Opäť vieme pridať like za iného užívatela len tým že zmeníme cookie header v requeste.

# Dodatok

Priamo skopírované POST requesty pre add_post/page_like sú v [new_post_original.sh](injection/new_post_original.sh) a [page_like_original.sh](injection/page_like_original.sh).

Requesty so zmenenými cookies sú v [new_post_stolen.sh](injection/new_post_stolen.sh) a [page_like_stolen.sh](injection/page_like_stolen.sh).

Vytvoril som aj skript, ktorý automaticky pošle príspevok a dá like na stránku pre všetky odchytené cookies: [like_and_post.py](injection/like_and_post.py).

Moja skupina `HackGroup` má celkovo 20 likov :D
