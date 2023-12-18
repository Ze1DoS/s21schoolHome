#include "grep_options.h"

int main(int argc, char *argv[]) {
  char *patterns[MAX_PATTERNS] = {0};
  char regex_pattern[MAX_LINE_LENGTH] = "";
  struct Flags grep_flags = {0};

  if (argc < 2) {
    printf("Usage: s21_grep [-flag] [pattern] [file ...]\n");
    return 1;
  }

  // Врубаем флаги
  cook_flags(argc, argv, &grep_flags);

  int pattern_count = 0;
  for (int i = 0; i < MAX_PATTERNS; i++) {
    if (patterns[i] != NULL) {
      pattern_count++;
    } else {
      break;
    }
  }
  // Обрабатываем паттерны
  pattern_count = cook_patterns(argc, argv, patterns, pattern_count,
                                &grep_flags, regex_pattern);
  if (pattern_count == -1) {
    return 1;
  }

  // Освобождаем память, выделенную под patterns
  for (int j = 0; j < pattern_count; j++) {
    if (patterns[j] != NULL) {
      free(patterns[j]);
      patterns[j] = NULL;
    }
  }

  // Итерируемся по файлам и ищем паттерны
  for (int i = optind + 1 - (grep_flags.eflag || grep_flags.fflag); i < argc;
       i++) {
    FILE *file = fopen(argv[i], "r");
    if (file == NULL) {
      if (!grep_flags.sflag) {
        fprintf(stderr, "s21_grep: %s : No such file or directory\n", argv[i]);
        exit(1);
      }
    } else {
      grep(file, argv[i], regex_pattern, &grep_flags,
           argc - optind - 1 + (grep_flags.eflag || grep_flags.fflag));
      fclose(file);
    }
  }

  return 0;
}
