#include "options.h"

void printline(struct flags *F, char *filename, char *line) {
  if (!F->o || F->c) F->lines++;
  if (!F->l && !F->c) {
    if (!F->o || F->printed != F->countlines || F->newfile) {
      if (F->files > 1 && !F->h) printf("%s:", filename);
      if (F->n) printf("%d:", F->countlines);
    }
    printf("%s", line);
    if (F->o) {
      F->printed = F->countlines;
      F->newfile = 0;
    }
  }
}

int compare(struct flags *F, char *pattern, char *filename, char *line) {
  regex_t regex;
  int result = 0;
  int icase = 0;
  if (F->i) icase = REG_ICASE;
  if (!regcomp(&regex, pattern, REG_NEWLINE | icase)) {
    regmatch_t *reg = malloc(sizeof(regmatch_t) * (regex.re_nsub + 1));
    int err = 0;
    err = regexec(&regex, line, regex.re_nsub + 1, reg, 0);
    if (!F->v && !err) {
      if (F->o) {
        do {
          char reg_line[5000];
          int len = reg[0].rm_eo - reg[0].rm_so;
          memcpy(reg_line, line + reg[0].rm_so, len);
          reg_line[len] = '\0';
          printline(F, filename, reg_line);
          if (!F->c && !F->l) putchar('\n');
          line += reg[0].rm_eo;
        } while (!regexec(&regex, line, regex.re_nsub + 1, reg, 0));
      } else {
        printline(F, filename, line);
      }
      result = 1;
    }
    if (F->v && err) result = 1;
    free(reg);
  }
  regfree(&regex);
  return result;
}

int regulars(struct flags *F, char **patterns, char *filename, char *line,
             int count, const int *files_f) {
  int result = 0;
  int vcount = 0;
  int allcount = count;
  for (int i = 0; !result && i < count; i++) {
    if (F->f && files_f[i] == 1) {
      allcount--;
      FILE *fp = NULL;
      if ((fp = fopen(patterns[i], "r")) == NULL) {
        if (!F->s) {
          fprintf(stderr, "grep: %s: No such file or directory\n", patterns[i]);
          result = 2;
        }
      } else {
        size_t len;
        char *pat = NULL;
        while (getline(&pat, &len, fp) != EOF) {
          pat[strlen(pat) - 1] = '\0';
          if (!F->v) result = compare(F, pat, filename, line);
          if (result && !F->o) break;
          if (F->v) {
            allcount++;
            vcount += compare(F, pat, filename, line);
          }
        }
        free(pat);
        fclose(fp);
      }
    } else {
      if (!F->v) result = compare(F, patterns[i], filename, line);
      if (F->v) vcount += compare(F, patterns[i], filename, line);
    }
    if (F->v && vcount == allcount) {
      printline(F, filename, line);
      result = 1;
    }
  }
  return result;
}

int s21_grep(struct flags *F, char **pattern, char *filename, int count,
             int *files_f) {
  FILE *fp = NULL;
  int res = 1;
  if ((fp = fopen(filename, "r")) == NULL) {
    F->files--;
    if (!F->s)
      fprintf(stderr, "grep: %s: No such file or directory\n", filename);
    return 0;
  }
  F->newfile = 1;
  size_t len;
  char *line = NULL;
  while (getline(&line, &len, fp) != EOF) {
    F->countlines++;
    res = regulars(F, pattern, filename, line, count, files_f);
    if ((res == 1 && F->l) || res == 2) break;
  }
  if (F->c) {
    if (F->files > 1) printf("%s:", filename);
    printf("%d\n", F->lines);
  }
  if (F->l && F->lines) printf("%s\n", filename);
  F->lines = F->countlines = 0;
  free(line);
  fclose(fp);
  return res;
}

int invalid_option(char c) {
  fprintf(stderr,
          "grep: invalid option -- %c\nusage: grep "
          "[-abcDEFGHhIiJLlmnOoqRSsUVvwxZ] [-A num] [-B num] [-C[num]]\n       "
          " [-e pattern] [-f file] [--binary-files=value] [--color=when]\n     "
          "   [--context[=num]] [--directories=action] [--label] "
          "[--line-buffered]\n        [--null] [pattern] [file ...]",
          c);
  return 0;
}

int parser(struct flags *F, char *args) {
  int result = 1;
  size_t len = strlen(args);
  for (size_t j = 1; j < len && result; j++) {
    if (args[j] == 'e') {
      F->e = 1;
      break;
    }
    if (args[j] == 'c')
      F->c = 1;
    else if (args[j] == 'l')
      F->l = 1;
    else if (args[j] == 'v')
      F->v = 1;
    else if (args[j] == 'i')
      F->i = 1;
    else if (args[j] == 'n')
      F->n = 1;
    else if (args[j] == 'h')
      F->h = 1;
    else if (args[j] == 's')
      F->s = 1;
    else if (args[j] == 'f')
      F->f = 1;
    else if (args[j] == 'o')
      F->o = 1;
    else
      result = invalid_option(args[j]);
  }
  return result;
}

void search_files(struct flags *F, int argc, char **argv, char **patterns,
                  int count, int *files_f) {
  for (int j = 0; j < argc; j++) {
    if (argv[j][0] != '-') {
      int ispattern = 0;
      for (int i = 0; i < count && !ispattern; i++)
        if (argv[j] == patterns[i]) ispattern = 1;
      if (!ispattern)
        if (s21_grep(F, patterns, argv[j], count, files_f) == 2) break;
    }
  }
}

int myrealloc(char ***patterns, int **files_f, int *n) {
  int result = 1;
  *n += 1024;
  char **tmp_pat = NULL;
  tmp_pat = realloc(*patterns, *n * sizeof(char *));
  if (tmp_pat)
    *patterns = tmp_pat;
  else
    result = 0;
  int *tmp_files = NULL;
  tmp_files = realloc(*files_f, *n * sizeof(int));
  if (tmp_files)
    *files_f = tmp_files;
  else
    result = 0;
  return result;
}

void few_patterns(struct flags *F, int argc, char **argv) {
  int n = 1024;
  char **patterns = (char **)malloc(n * sizeof(char *));
  int *files_f = calloc(n, sizeof(int));
  if (files_f && patterns) {
  int count = 0;
  for (int i = 0; i < argc; i++) {
    if (argv[i][0] == '-') {
      char *e = strchr(argv[i], 'e');
      char *f = strchr(argv[i], 'f');
      if (e) {
        if (e[1] != '\0')
          argv[i] = (e + 1);
        else
          i++;
        if (i >= argc) break;
        patterns[count] = argv[i];
        count++;
      } else if (f) {
        if (f[1] != '\0')
          argv[i] = (f + 1);
        else
          i++;
        if (i >= argc) break;
        patterns[count] = argv[i];
        files_f[count] = 1;
        count++;
      }
      if (count > n - 5) break;
    }
  }
  if (count) search_files(F, argc, argv, patterns, count, files_f);
  }
  free(patterns);
  free(files_f);
}

void one_pattern(struct flags *F, int argc, char **argv) {
  char *pattern = NULL;
  for (int i = 0; i < argc; i++)
    if (argv[i][0] != '-') {
      if (!pattern)
        pattern = argv[i];
      else
        s21_grep(F, &pattern, argv[i], 1, (int *)pattern);
    }
}

void countfiles(struct flags *F, int argc, char **argv) {
  if (F->e && **argv != '-') F->files++;
  for (int i = 1; i < argc; i++)
    if (argv[i][0] != '-' &&
        (argv[i - 1][0] != '-' ||
         (strchr(argv[i - 1], 'e') && *(strchr(argv[i - 1], 'e') + 1) != '\0')))
      F->files++;
}

void s21_getopt(struct flags *F, int argc, char **argv) {
  int result = 1;
  for (int i = 0; i < argc && result; i++)
    if (argv[i][0] == '-' && (!i || !strchr(argv[i - 1], 'e')))
      result = parser(F, argv[i]);
  if (result) {
    countfiles(F, argc, argv);
    if (F->e || F->f)
      few_patterns(F, argc, argv);
    else
      one_pattern(F, argc, argv);
  }
}

