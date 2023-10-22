#include <stdio.h>
#include <string.h>

typedef int bool; // псевдоним для типа int, для удобства

void print_symb(int c, int *prev, char *flags, int *index, bool *eline_printed);
int print_file(char *name, char *flags);
void flags_parser(char *flags, int argc, char **argv, int *index);
void append_flags(char *flags, char flag);
void append_flag(char *flags, char flag);

int main(int argc, char **argv) {
    char flags[7] = "\0"; // массив флагов
    int index_end_flags = 1;
    flags_parser(flags, argc, argv, &index_end_flags);
    
    // если не было подано файла
    if (index_end_flags == argc - 1) {
        print_file("-", flags); // считываем содержимое стандартного ввода
    } else {
        for (int i = index_end_flags + 1; i < argc; i++) {
            print_file(argv[i], flags); // выводим содержимое каждого файла
        }
    }
    
    return 0;
}

void print_symb(int c, int *prev, char *flags, int *index, bool *eline_printed) {
    // если s и это пустая строка и пустая строка уже была выведена, пропустим, сюда не зайдем
    if (!(strchr(flags, 's') != NULL && *prev == '\n' && c == '\n' && *eline_printed)) {
        
        if (*prev == '\n' && c == '\n') *eline_printed = 1;
        else *eline_printed = 0;
        
        // если ( (n без b) или (b и текущий символ не '\n') ) и пред символ '\n'
        if (((strchr(flags, 'n') != NULL && strchr(flags, 'b') == NULL) || (strchr(flags, 'b') != NULL && c != '\n')) && *prev == '\n') {
            (*index)++;
            printf("%6d\t", *index);
        }
        
        if (strchr(flags, 'E') != NULL && c == '\n') printf("$");
        if (strchr(flags, 'T') != NULL && c == '\t') {
            printf("^");
            c = '\t' + 64;
        }
        if (strchr(flags, 'v') != NULL) {
            if (c > 127 && c < 160) printf("M-^");
            if ((c < 32 && c != '\n' && c != '\t') || c == 127) printf("^");
            if ((c < 32 || (c > 126 && c < 160)) && c != '\n' && c != '\t') c = c > 126 ? c - 128 + 64 : c + 64;
        }
        fputc(c, stdout);
    }
    *prev = c;
}

int print_file(char *name, char *flags) {
    int err_code = 0;
    FILE *f;
    
    if (strcmp(name, "-") == 0) {
        f = stdin; // чтение из стандартного ввода, если имя файла "-"
    } else {
        f = fopen(name, "rt"); // открытие файла для чтения
    }
    
    if (f == NULL) {
        printf("cat: %s: No such file or directory\n", name);
        err_code = 1;
    } else {
        int c;
        int prev = '\n';
        bool eline_printed = 0;
        int index = 0; // Объявление переменной index

        while ((c = fgetc(f)) != EOF) {
            print_symb(c, &prev, flags, &index, &eline_printed);
        }
        
        if (fclose(f) != 0) {
            printf("cat: Error closing file\n");
            err_code = 1;
        }
    }
    
    return err_code;
}


void flags_parser(char *flags, int argc, char **argv, int *index) {

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] == '-') {
            if (arg[1] == '\0') append_flags(flags, arg[1]);
            else {
                for (size_t j = 1; j < strlen(arg); j++) {
                    if (arg[j] == '-') append_flag(flags, arg[j+1]);
                    else append_flags(flags, arg[j]);
                }
            }
            *index = i;
        } else {
            break;
        }
    }
}

void append_flags(char *flags, char flag) {
    if (strchr(flags, flag) == NULL) {
        size_t len = strlen(flags);
        flags[len] = flag;
    }
}

void append_flag(char *flags, char flag) {
    if (strchr(flags, flag) == NULL) {
        size_t len = strlen(flags);
        flags[len] = '-';
        flags[len+1] = flag;
    }
}
