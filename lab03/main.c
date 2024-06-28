#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>


#define WARN(msg) fprintf(stderr, "%s\n", msg)

/**
 * Read line or maximum of buff_size - 1 characters from stdin.
 *
 * @param buffer (char *) Output buffer.
 * @param buff_size (size_t) Size of buffer.
 * @return Number of read bytes(not including '\n' or '\0'), on success.
 *         -1 on stream error.
 */
int my_gets(char *buffer, size_t buff_size) {
    if(buff_size == 0)
        return -1;

    int chars_read = 0;
    while (chars_read < buff_size - 1) {
        int c = getc(stdin);
        if(c == EOF || c == '\n')
            break;

        buffer[chars_read++] = c;
    }
    if(chars_read == buff_size - 1) {
        int c = getc(stdin);
        if(c != '\n')
            ungetc(c, stdin);
    }
    buffer[chars_read] = '\0';
    if(ferror(stdin))
        return -1;

    return chars_read;
}

/**
 * Generates random word from supplied characters.
 * @param word
 * @param word_len
 * @param buffer_chars
 * @param chars_cnt
 */
void generate_word(char word[], int word_len,
                   const char buffer_chars[], int chars_cnt) {
    for(int i = 0; i < word_len; i++)
        word[i] = buffer_chars[rand() % chars_cnt];
    word[word_len] = '\0';
}

/**
 * Shuffles characters in word.
 * @param word
 * @param word_len
 */
void shuffle(char word[], int word_len) {
    for(int i = 0; i < word_len; i++) {
        char temp = word[i];
        int r = rand() % word_len;
        word[i] = word[r];
        word[r] = temp;
    }
}


// here is bug #1:
// game allows to compare guess with data behind word buffer leading to
// exposure of data(canary and addresses)
int game(char word[]) {
    int g_len;
    int correct, wrong_place, wrong;

    printf("# - correct\n~ - wrong place\n. - wrong\n\n");
    do {
        correct = wrong_place = wrong = 0;
        const int MAX_GUESS_LEN = 256;
        char guess[MAX_GUESS_LEN];
        char answer[MAX_GUESS_LEN];
        memset(answer, '.', MAX_GUESS_LEN);

        const int ASCII_CNT = 256;
        char bins[ASCII_CNT];
        memset(bins, 0, ASCII_CNT);

        printf("Guess the word: ");
        fflush(stdout);
        g_len = my_gets(guess, MAX_GUESS_LEN);
        if(g_len < 0) {
            return -1;
        }

        if(g_len == 0) {
            printf("End guessing...\n");
            printf("Answer was: %s\n", word);
            return 0;
        }

        for(int i = 0; i < g_len; i++) {
            if(word[i] == guess[i]) {
                answer[i] = '#';
                correct++;  // count how many letters were correct and at right place
            }
            else {
                wrong++;
                bins[(uint8_t)word[i]]++;  // keep track of non-guessed characters
            }
        }

        for(int i = 0; i < g_len; i++) {
            // not at right place but is somewhere else
            if(word[i] != guess[i] && bins[(uint8_t)guess[i]]) {
                answer[i] = '~';
                wrong_place++;
                bins[(uint8_t)guess[i]]--;
            }
        }

        answer[g_len] = '\0';
        printf("                %s\n", answer);
        fflush(stdout);
    } while (wrong != 0 || strlen(word) != g_len);

    printf("Good work correct answer is: %s\n", word);

    return 0;
}

typedef enum MODE {
    MODE_GENERATED, MODE_SHUFFLE
} MODE_T;


int main(int argc, char *argv[]) {
    srand(0);
    printf("Guess the word game:\n\n");

    printf("How to play:\n");
    printf("There are two game modes: (1.) guess generated word / (2.) shuffle word.\n");
    printf("------------------------------------------------------------------------\n");
    printf("1. Guess generated word:\n");
    printf("  - You enter length of word to generate,\n");
    printf("  - random word from lower alphabet will be generated.\n");
    printf("------------------------------------------------------------------------\n");
    printf("2. Shuffle word:\n");
    printf("  - You enter length of word with negative sign.\n");
    printf("  - then you enter word to shuffle.\n");
    printf("------------------------------------------------------------------------\n\n");

    while(1) {
        const int MAX_BUFFER_SIZE = 32;
        char word[MAX_BUFFER_SIZE];
        int word_len;
        char _newline;
        MODE_T mode = MODE_GENERATED;

        printf("Enter word length or \"0\" to end game: ");
        fflush(stdout);

        if(scanf("%d%c", &word_len, &_newline) != 2) {
            printf("Input length not read. Exiting\n");
            return 1;
        }

        if(word_len == 0)
            break;

        else if(word_len < -1) {
            mode = MODE_SHUFFLE;
            // bug #2, we are using negative sign to store information about game mode,
            // but we are not counting with INT32_MIN integer overflow in * -1.
            word_len *= -1;
        }

        // space for trailing '\0'
        if(MAX_BUFFER_SIZE - 1 < word_len) {
            printf("Word too long (%d), try again below %d.\n",
                   word_len, MAX_BUFFER_SIZE);
            continue;
        }

        if(mode == MODE_GENERATED) {
            int MAX_CHARS = 256;
            char buffer_chars[MAX_CHARS];
            int chars_cnt = 0;

//            printf("Choose characters(or ENTER to choose whole alphabet): ");
//            int chars_cnt = my_gets(buffer_chars, sizeof(buffer_chars));
//
//            if(chars_cnt == -1) {
//                WARN("Could not read input. Exiting.");
//                return 1;
//            }
            if(chars_cnt == 0) {
                char *bc = buffer_chars;
                for(char c = 'a'; c <='z'; c++)
                    *(bc++) = c;
                *bc = '\0';
                chars_cnt = 'z' - 'a' + 1;
            }

            generate_word(word, word_len, buffer_chars, chars_cnt);
        }
        else {
            printf("Enter your word: ");
            fflush(stdout);
            int read_len = my_gets(word, word_len + 1);  // +1 for trailing '\0'

            if(read_len == -1) {
                WARN("Could not read input. Exiting.");
                return 1;
            }

            if(read_len != word_len) {
                printf("Word too short %d, needed %d.\nTry again\n",
                       read_len, word_len);
                continue;
            }

            int ret = fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
            if(ret < 0) {
                WARN("Could not read input. Exiting.");
                return 1;
            }

            while(getc(stdin) != EOF);  // read excess characters

            ret = fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) & ~O_NONBLOCK);
            clearerr(stdin);

            if(ret < 0) {
                WARN("Could not read input. Exiting.");
                return 1;
            }


            shuffle(word, word_len);
        }

        if(game(word) == -1) {
            WARN("Error occurred. Exiting.\n");
            return 1;
        }
    }

    printf("Thanks for playing.\n");

    return 0;
}


// functions that simulate big code base with hundreds of function we can choose from
void func_1(const char *cmd,
            char * const *argv,
            char * const *envp) {
/*      some random code before exec      */
/*      never executed, neither in code   */
/*      nor in the attack                 */
/**/    int a;                          /**/
/**/    printf("Enter number: ");       /**/
/**/    if(scanf("%d", &a) != 1)        /**/
/**/        exit(1);                    /**/
/**/                                    /**/
/**/    // Collatz conjecture           /**/
/**/    while(a != 1) {                 /**/
/**/        if(a%2 == 0)                /**/
/**/            a /= 2;                 /**/
/**/        else                        /**/
/**/            a = 3 * a + 1;          /**/
/**/    }                               /**/
/**/                                    /**/
/**/    pid_t pid = fork();             /**/
/**/    if(pid == -1) {                 /**/
/**/        WARN("Fork failed.");       /**/
/**/        return;                     /**/
/**/    }                               /**/
/**/                                    /**/
/**/    if(pid == 0) { // I am child    /**/
/*      some random code before exec      */

        execve(cmd, argv, envp);

/*      some random code after exec                    */
/*      never executed, neither in code                */
/*      nor in the attack                              */
/**/    }                                            /**/
/**/                                                 /**/
/**/    int pmax = 200, pcount = 0;                  /**/
/**/    int eratosthenes[pmax];                      /**/
/**/    memset(eratosthenes, 1, pmax);               /**/
/**/                                                 /**/
/**/    pcount++;                                    /**/
/**/    for(int i = 2; i < pmax; i++)                /**/
/**/        eratosthenes[i] = 2;                     /**/
/**/                                                 /**/
/**/    for(int i = 3; i*i <= pmax; i += 2) {        /**/
/**/        if(eratosthenes[i] != 1) continue;       /**/
/**/        pcount++;                                /**/
/**/        for(int j = i*i; j < pmax; j += 2*i) {   /**/
/**/            eratosthenes[j] = i;                 /**/
/**/        }                                        /**/
/**/    }                                            /**/
/**/    printf("%d primes below %d", pcount, pmax);  /**/
/*      some random code after exec                    */
}
