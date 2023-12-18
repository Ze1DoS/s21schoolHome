#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

typedef struct {
  const char *filename;
  const char *template;
  bool flag_i;
  bool flag_v;
  bool flag_c;
  bool flag_n;
  bool flag_l;
  bool flag_o;
  bool flag_f;
  bool flag_s;
  bool flag_h;
} Options;

// Функция для анализа и обработки флагов командной строки
void parse_arguments(int argc, char *argv[], Options *options) {
  if (argc < 3) {
    printf("Ошибка: недостаточно аргументов\n");
    exit(1);
  }

  options->filename = argv[1];

  int flag_count = argc - 3; // Количество флагов

  // Установка параметров по умолчанию
  options->flag_i = false;
  options->flag_v = false;
  options->flag_c = false;
  options->flag_n = false;
  options->flag_l = false;
  options->flag_o = false;
  options->flag_f = false;
  options->flag_s = false;
  options->flag_h = false;

  if (strcmp(argv[2], "-i") == 0) {
    // Флаг -i: игнорировать регистр
    options->flag_i = true;
    flag_count--;
  } else if (strcmp(argv[2], "-v") == 0) {
    // Флаг -v: вывести строки, не содержащие совпадений
    options->flag_v = true;
    flag_count--;
  } else if (strcmp(argv[2], "-c") == 0) {
    // Флаг -c: вывести количество совпадений
    options->flag_c = true;
    flag_count--;
  } else if (strcmp(argv[2], "-n") == 0) {
    // Флаг -n: вывести номера строк
    options->flag_n = true;
    flag_count--;
  } else if (strcmp(argv[2], "-l") == 0) {
    // Флаг -l: вывести имена файлов
    options->flag_l = true;
    flag_count--;
  } else if (strcmp(argv[2], "-o") == 0) {
    // Флаг -o: вывести только совпавшую часть строки
    options->flag_o = true;
    flag_count--;
  } else if (strcmp(argv[2], "-f") == 0) {
    // Флаг -f: использовать файл с шаблонами
    options->flag_f = true;
    flag_count--;
  } else if (strcmp(argv[2], "-s") == 0) {
    // Флаг -s: подавить сообщения об ошибках
    options->flag_s = true;
    flag_count--;
  } else if (strcmp(argv[2], "-h") == 0) {
    // Флаг -h: подавить имена файлов
    options->flag_h = true;
    flag_count--;
  }

  options->template = argv[2 + flag_count];

  if (flag_count > 0) {
    printf("Ошибка: неизвестные флаги\n");
    exit(1);
  }
}

// Функция для поиска совпадений с помощью регулярного выражения
bool regex_match(const char *line, const char *regex, Options *options) {
  regex_t re;
  regmatch_t match;

  int flags = REG_EXTENDED;
  if (options->flag_i) {
    flags |= REG_ICASE;
  }

  int ret = regcomp(&re, regex, flags);
  if (ret != 0) {
    printf("Ошибка: некорректное регулярное выражение\n");
    exit(1);
  }

  ret = regexec(&re, line, 1, &match, 0);
  regfree(&re);

  if (ret == 0) {
    // Соответствие найдено
    if (options->flag_v) {
      return false;
    }
    return true;
  } else if (ret == REG_NOMATCH) {
    // Соответствия не найдено
    if (options->flag_v) {
      return true;
    }
    return false;
  } else {
    // Ошибка выполнения регулярного выражения
    printf("Ошибка: ошибка выполнения регулярного выражения\n");
    exit(1);
  }
}

// Вывод строки с подстветкой совпадающей части
void print_matched_line(const char *line, regmatch_t match) {
  printf("%.*s", match.rm_so, line); // Вывод части строки до совпадения
  printf("\033[1m\033[32m"); // Установка жирного зеленого цвета
  printf("%.*s", match.rm_eo - match.rm_so,
         line + match.rm_so); // Вывод совпавшей части строки
  printf("\033[0m"); // Сброс настроек форматирования
  printf("%s", line + match.rm_eo); // Вывод оставшейся части строки
}

// Поиск соответствий для указанного шаблона
void grep_template(const char *template, Options *options) {
     regmatch_t match;
  if (options->filename != NULL && options->flag_h) {
    // Вывод только имени файла для флага -h
    printf("%s\n", options->filename);
    return;
  }

  FILE *file = fopen(options->filename, "r");
  if (file == NULL) {
    if (!options->flag_s) {
      printf("Ошибка: не удалось открыть файл %s\n", options->filename);
    }
    return;
  }

  char line[MAX_LINE_LENGTH];
  bool match_found = false;
  int line_number = 0;
  int match_count = 0;

  while (fgets(line, sizeof(line), file)) {
    line_number++;

    if (options->flag_i) {
      // Приведение строки к нижнему регистру
      // ...
    }

    if (regex_match(line, template, options)) {
      match_found = true;
      match_count++;

      if (options->flag_l) {
        // Вывод имени файла и прекращение обработки файла
        if (!options->flag_h) {
          printf("%s\n", options->filename);
        }
        break;
      } else {
        if (!options->flag_h) {
          // Вывод имени файла перед совпадающей строкой
          printf("%s:", options->filename);
        }

        if (options->flag_n) {
          // Вывод номера строки
          printf("%d:", line_number);
        }

        if (options->flag_o) {
          // Вывод только совпавшей части строки
          print_matched_line(line, match);
        } else {
          // Вывод совпавшей строки целиком
          printf("%s", line);
        }
      }

      if (options->flag_c) {
        // Прекращение обработки файла после первого совпадения для флага -c
        break;
      }
    }
  }

  fclose(file);

  if (options->flag_c && match_found) {
    // Вывод количества совпадений для флага -c
    printf("%s: %d\n", options->filename, match_count);
  } else if (!match_found && options->flag_l) {
    // Вывод "файл не найден" для флага -l
    printf("%s\n", options->filename);
  }
}

// Поиск соответствий для файла с шаблонами
void grep_file(const char *filename, Options *options) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Ошибка: не удалось открыть файл %s\n", filename);
    exit(1);
  }

  char template[MAX_LINE_LENGTH];

  while (fgets(template, sizeof(template), file)) {
    template[strcspn(template, "\n")] = '\0'; // Удаление символа новой строки

    if (options->flag_s && options->flag_h) {
      // Подавление сообщений об ошибке
      continue;
    }

    if (!options->flag_h) {
      printf("Шаблон: %s\n", template);
    }

    options->template = template;

    // Поиск соответствий для текущего шаблона
    grep_template(options->template, options);
  }

  fclose(file);
}

// Обновленная функция main
int main(int argc, char *argv[]) {
  Options options;

  // Анализ и обработка флагов командной строки
  parse_arguments(argc, argv, &options);

  // Поиск соответствий для каждого указанного файла или шаблона
  if (options.flag_f) {
    // Чтение списка регулярных выражений из файла
    grep_file(options.filename, &options);
  } else {
    // Поиск соответствий для указанного файла или шаблона
    grep_template(options.template, &options);
  }

  return 0;
}