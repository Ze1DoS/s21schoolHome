I apologize for the error. The `is_regex_pattern` variable is indeed unused in the code. You can remove it to resolve the compilation error. Here's the updated code:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre.h>

// Размер буфера для чтения строки
#define BUFFER_SIZE 4096

// Функция для проверки, содержит ли строка pattern регулярное выражение
int is_regex(const char* pattern) {
    return (strchr(pattern, '\\') != NULL) || (strchr(pattern, '*') != NULL) || (strchr(pattern, '.') != NULL);
}

// Функция для поиска совпадений в строке
int grep_line(const char* line, const char* pattern, int ignore_case) {
    pcre* regex;
    const char* error;
    int erroffset;
    int ovector[30];

    // Компилируем регулярное выражение
    int options = PCRE_MULTILINE;
    if (ignore_case)
        options |= PCRE_CASELESS;
    regex = pcre_compile(pattern, options, &error, &erroffset, NULL);
    if (!regex) {
        fprintf(stderr, "Ошибка компиляции регулярного выражения: %s\n", error);
        return 0;
    }

    // Ищем совпадения в строке
    int ret = pcre_exec(regex, NULL, line, strlen(line), 0, 0, ovector, sizeof(ovector)/sizeof(int));
    if (ret >= 0) {
        pcre_free(regex);
        return 1;
    } else if (ret == PCRE_ERROR_NOMATCH) {
        pcre_free(regex);
        return 0;
    } else {
        fprintf(stderr, "Ошибка выполнения регулярного выражения: %d\n", ret);
        pcre_free(regex);
        return 0;
    }
}

// Функция для поиска совпадений в файле
int grep_file(const char* filename, const char* pattern, int ignore_case, int invert_match, int count_matches, int print_line_numbers) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Не удалось открыть файл: %s\n", filename);
        return 0;
    }

    char buffer[BUFFER_SIZE];
    int line_number = 0;
    int match_count = 0;

    while (fgets(buffer, BUFFER_SIZE, file)) {
        line_number++;

        // Удаляем символ новой строки
        buffer[strcspn(buffer, "\n")] = 0;

        // Выполняем поиск совпадений
        int match = grep_line(buffer, pattern, ignore_case);

        if ((!match && !invert_match) || (match && invert_match)) {
            if (count_matches) {
                match_count++;
            } else {
                if (print_line_numbers) {
                    printf("%s:%d:%s\n", filename, line_number, buffer);
                } else {
                    printf("%s:%s\n", filename, buffer);
                }
            }
        }
    }

    fclose(file);

    if (count_matches) {
        printf("%s: %d\n", filename, match_count);
    }

    return 1;
}

int main(int argc, char* argv[]) {
    // Проверяем наличие аргументов командной строки
    if (argc < 3) {
        fprintf(stderr, "Использование: %s <паттерн> <файл1> [<файл2> ...]\n", argv[0]);
        return 1;
    }

    // Устанавливаем флаги по умолчанию
    int ignore_case = 0;
    int invert_match = 0;
    int count_matches = 0;
    int print_line_numbers = 0;

    // Парсим флаги командной строки
    int i;
    for (i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--ignore-case") == 0) {
            ignore_case = 1;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--invert-match") == 0) {
            invert_match = 1;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--count") == 0) {
            count_matches = 1;
        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--files-with-matches") == 0) {
            print_line_numbers = 1;
        } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--line-number") == 0) {
            print_line_numbers = 1;
        }
    }

    // Парсим паттерн
    const char* pattern = argv[i];

    // Перебираем файлы для поиска совпадений
    for (i = i + 1; i < argc; i++) {
        const char* filename = argv[i];
        if (grep_file(filename, pattern, ignore_case, invert_match, count_matches, print_line_numbers)) {
            printf("\n");
        }
    }

    return 0;
}
```

After making this change, you should be able to compile the code without any errors.
