#include "grep_options.h"

// Парсер командной строки
void parseCommandLine(int argc, char *argv[], struct Options *grep_options) {
  int opt;
  while ((opt = getopt(argc, argv, "e:ivclnhsof:?")) != -1) {
    switch (opt) {
    case 'e':
      grep_options->extendedPattern = 1;
      break;
    case 'i':
      grep_options->ignoreCase = 1;
      break;
    case 'v':
      grep_options->invertMatch = 1;
      break;
    case 'c':
      grep_options->countMatches = 1;
      break;
    case 'l':
      grep_options->listFiles = 1;
      break;
    case 'n':
      grep_options->lineNumbers = 1;
      break;
    case 'h':
      grep_options->hideFileNames = 1;
      break;
    case 's':
      grep_options->silentMode = 1;
      break;
    case 'f':
      grep_options->filePattern = 1;
      break;
    case 'o':
      grep_options->onlyMatched = 1;
      break;
    case '?':
      printf("Usage: s21_grep [-flag] [pattern] [file ...]\n");
      exit(1);
    }
  }
}

// Подготовка паттернов и их слияние по дефолту + подготовка паттернов из файла
int preparePatterns(int argc, char *argv[], char *patterns[], int patternCount,
                    struct Options *grep_options, char *regexPattern) {
  int i = 0;

  // Обработка паттернов из файла
  while (i < argc) {
    if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
      i++;
      FILE *file = fopen(argv[i], "r");
      if (file != NULL) {
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {
          char *newline = strchr(line, '\n');
          if (newline != NULL) {
            *newline = '\0';
          }
          if (patternCount < MAX_PATTERNS) {
            patterns[patternCount] = strdup(line);
            patternCount++;
          }
        }
        fclose(file);
      } else {
        fprintf(stderr, "Error opening file: %s\n", argv[i]);
      }
    } else if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
      i++;
      if (patternCount < MAX_PATTERNS) {
        patterns[patternCount] = strdup(argv[i]);
        patternCount++;
      } else {
        fprintf(stderr, "Maximum number of patterns reached.\n");
        return -1;
      }
    }
    i++;
  }

  // Обработка паттернов из командной строки
  if (patternCount == 0 && !grep_options->filePattern) {
    if (optind < argc) {
      if (patternCount < MAX_PATTERNS) {
        patterns[patternCount] = strdup(argv[optind]);
        patternCount++;
      } else {
        fprintf(stderr, "Maximum number of patterns reached.\n");
        return -1;
      }
    } else {
      fprintf(stderr, "No patterns specified.\n");
      return -1;
    }
  }

  regexPattern[0] = '\0';

  for (int j = 0; j < patternCount; j++) {
    if (j > 0) {
      strcat(regexPattern, "|");
    }
    strcat(regexPattern, patterns[j]);
  }

  return patternCount;
}

// Проверка файла
int isRegularFile(const char *path) {
  struct stat pathStat;
  if (stat(path, &pathStat) != 0) {
    return 0;
  } else {
    return S_ISREG(pathStat.st_mode);
  }
}

// База
int grep(FILE *file, const char *filename, const char *pattern,
         struct Options *grep_options, int fileCount) {
  regex_t regex; // Структура под регулярку

  if (!compileRegex(pattern, &regex, grep_options)) {
    return 0;
  }

  bool filenamePrinted = false;
  bool matchedInFile = false;

  int count = 0;

  if (grep_options->onlyMatched) {
    count =
        searchForPatternWithOflag(file, filename, &regex, grep_options,
                                  fileCount, &filenamePrinted, &matchedInFile);
  } else {
    count = searchForPattern(file, filename, &regex, grep_options, fileCount,
                             &filenamePrinted, &matchedInFile);
  }

  regfree(&regex);

  printResult(grep_options, filename, fileCount, count, matchedInFile);

  return count;
}

// Поиск паттернов для -o флага
int searchForPatternWithOflag(FILE *file, const char *filename, regex_t *regex,
                              struct Options *grep_options, int fileCount,
                              bool *filenamePrinted, bool *matchedInFile) {
  char line[MAX_LINE_LENGTH];
  int lineNumber = 0;
  int count = 0;

  while (fgets(line, sizeof(line), file) != NULL) {
    lineNumber++;

    regmatch_t matches[1];
    int offset = 0;
    bool matchedInLine = false;

    while (regexec(regex, line + offset, 1, matches, 0) == 0) {
      int start = matches[0].rm_so;
      int end = matches[0].rm_eo;
      char *matchedWord = strndup(line + offset + start, end - start);
      matchedInLine = true;

      if (grep_options->listFiles && grep_options->countMatches &&
          matchedInLine) {
        *matchedInFile = true;
        return count;
      }

      if (!grep_options->countMatches && matchedInLine &&
          !grep_options->invertMatch) {
        *filenamePrinted = printLine(matchedWord, lineNumber, filename,
                                     grep_options, fileCount, *filenamePrinted);
      }
      offset += end;
      free(matchedWord);
    }

    if (grep_options->countMatches && matchedInLine) {
      count++;
    }

    if (!matchedInLine && grep_options->invertMatch) {
      *filenamePrinted = printLine(line, lineNumber, filename, grep_options,
                                   fileCount, *filenamePrinted);
    }

    if (*filenamePrinted) {
      break;
    }
  }

  return count;
}

// Поиск паттернов на дефолте
int searchForPattern(FILE *file, const char *filename, regex_t *regex,
                     struct Options *grep_options, int fileCount,
                     bool *filenamePrinted, bool *matchedInFile) {
  char line[MAX_LINE_LENGTH];
  int line_number = 0;
  int count = 0;

  while (fgets(line, sizeof(line), file) != NULL) {
    line_number++;

    bool matched = (regexec(regex, line, 0, NULL, 0) == 0);

    if (grep_options->listFiles && grep_options->countMatches && matched) {
      *matchedInFile = true;
      return count;
    }

    if ((matched && !grep_options->invertMatch) ||
        (!matched && grep_options->invertMatch)) {
      if (grep_options->countMatches) {
        count++;
      } else {
        *filenamePrinted = printLine(line, line_number, filename, grep_options,
                                     fileCount, *filenamePrinted);
      }
    }

    if (*matchedInFile || *filenamePrinted) {
      break;
    }
  }

  return count;
}

// Компиляция регулярного выражения
bool compileRegex(const char *pattern, regex_t *regex,
                  struct Options *grep_options) {
  int cflags = REG_EXTENDED;
  if (grep_options->ignoreCase) {
    cflags |= REG_ICASE;
  }
  if (grep_options->extendedPattern) {
    cflags |= REG_NEWLINE;
  }

  if (regcomp(regex, pattern, cflags) != 0) {
    fprintf(stderr, "Error compiling regular expression: %s\n", pattern);
    return false;
  }

  return true;
}

// Вывод строки с соответствующим форматированием
bool printLine(const char *line, int lineNumber, const char *filename,
               struct Options *grep_options, int fileCount,
               bool filenamePrinted) {
  if (!filenamePrinted) {
    if (!grep_options->silentMode &&
        (fileCount > 1 || !grep_options->hideFileNames)) {
      printf("%s:\n", filename);
    }
    filenamePrinted = true;
  }

  if (grep_options->lineNumbers) {
    printf("%d:", lineNumber);
  }

  printf("%s", line);

  return filenamePrinted;
}

// Вывод результата анализа
void printResult(struct Options *grep_options, const char *filename,
                 int fileCount, int count, bool matchedInFile) {
  if (grep_options->countMatches) {
    if (grep_options->listFiles && matchedInFile) {
      printf("%s\n", filename);
    }

    if (fileCount > 1 && !grep_options->listFiles) {
      printf("%s: %d\n", filename, count);
    } else if (!grep_options->listFiles) {
      printf("%d\n", count);
    }
  }
}

/*
int main(int argc, char *argv[]) {
  struct Options grep_options = {0};

  parseCommandLine(argc, argv, &grep_options);

  char *patterns[MAX_PATTERNS] = {NULL};
  int patternCount = preparePatterns(argc, argv, patterns, 0, &grep_options,
                                    grep_options.pattern);

  if (patternCount == -1) {
    return 1;
  }

  // Если нет указанных файлов, или вместо них прочитан -f
  if (optind >= argc || grep_options.filePattern) {
    grep(stdin, "(standard input)", grep_options.pattern, &grep_options, 1);
  } else {
    for (int i = optind; i < argc; i++) {
      const char *filename = argv[i];
      FILE *file = fopen(filename, "r");

      if (file == NULL) {
        fprintf(stderr, "No such file or directory: %s\n", filename);
        continue;
      }

      if (!isRegularFile(filename)) {
        fprintf(stderr, "Not a regular file: %s\n", filename);
        continue;
      }

      grep(file, filename, grep_options.pattern, &grep_options,
           argc - optind);
      fclose(file);
    }
  }

  return 0;
}
*/