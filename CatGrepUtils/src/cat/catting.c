#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include "table.h"

typedef struct
{
    bool numberNonEmpty;
    bool markEndl;
    bool numberAll;
    bool squeeze;
    bool tab;
    bool nonPrintable;
} Flags;

Flags readFlags(int argc, char *argv[]) {
    struct option longOption[] = {
        {"number", 0, NULL, 'n'},
        {"number-nonblank", 0, NULL, 'b'},
        {"squeeze-blank", 0, NULL, 's'},
        {NULL, 0, NULL, 0}
    };
    int currentFlag = getopt_long(argc, argv, "bevEnstT", longOption, NULL);
    Flags flags = {false, false, false, false, false, false};
    for (;currentFlag != -1; currentFlag = getopt_long(argc, argv, "bevEnstT", longOption, NULL)) {
        switch (currentFlag) {
            break; case 'b':
            flags.numberNonEmpty = true;
            break; case 'e':
            flags.markEndl = true;
            flags.nonPrintable = true;
            break; case 'v':
            flags.nonPrintable = true;
            break; case 'E':
            flags.markEndl = true;
            break; case 'n':
            flags.numberAll = true;
            break; case 's':
            flags.squeeze = true;
            break; case 't':
            flags.tab = true;
            flags.nonPrintable = true;
            break; case 'T':
            flags.tab = true;
        }
    }
    return flags;
}

void CatFile(FILE *file, Flags flag, const char *table[static 256]) {
    int ch = 0;
    int lch = 0;
    int lineNumber = 0;
    bool cureentLine = false;
    lch = '\n';
    (void) flag;
    while (fread(&ch, 1, 1, file) > 0){
        if (lch == '\n') {
            if(flag.squeeze && ch =='\n') {
                if(cureentLine)
                    continue;
                cureentLine = true;
            } else
                cureentLine = false;
            if(flag.numberNonEmpty) {
                if (ch != '\n') {
                    printf("%6i\t", ++lineNumber);
                }
            } else if (flag.numberAll) {
                printf("%6i\t", ++lineNumber);
            }
        }
        if (!*table[ch]){
            printf("%c", '\0');
        } else {
            printf("%s", table[ch]);
        }
        lch = ch;
    }
}

void cat(int argc, char *argv[], Flags flag, const char *table[static 256]){
    for(char **filename = &argv[1], **end = &argv[argc]; filename != end; ++filename) {
        if (**filename == '-')
            continue;
        FILE *file = fopen(*filename, "rb"); // чтение бинарника для винды
        if(errno) {
            fprintf(stderr, "%s", argv[0]);
            perror(*filename);
            continue;
        }
        CatFile(file, flag, table);
        fclose(file);
    }
}

int main(int argc, char *argv[]) {
    Flags flag = readFlags(argc, argv);
    const char *table[256];
    CatSetTable(table);
    if (flag.markEndl)
        CatEndl(table);
    if (flag.tab)
        CatTabs(table);
    if (flag.nonPrintable)
        CatUnprintable(table);
    cat(argc, argv, flag, table);
}