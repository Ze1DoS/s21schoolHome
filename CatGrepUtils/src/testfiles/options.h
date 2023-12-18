#ifndef OPTIONS_H
#define OPTIONS_H

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct flags {
  int c, l, e, v, i, o, n, h, s, f;
  int files, lines, countlines, newfile, printed;
};

void printline(struct flags *F, char *filename, char *line);
int compare(struct flags *F, char *pattern, char *filename, char *line);
int regulars(struct flags *F, char **patterns, char *filename, char *line,
             int count, const int *files_f);
int s21_grep(struct flags *F, char **pattern, char *filename, int count,
             int *files_f);
int invalid_option(char c);
int parser(struct flags *F, char *args);
void search_files(struct flags *F, int argc, char **argv, char **patterns,
                  int count, int *files_f);
int myrealloc(char ***patterns, int **files_f, int *n);
void few_patterns(struct flags *F, int argc, char **argv);
void one_pattern(struct flags *F, int argc, char **argv);
void countfiles(struct flags *F, int argc, char **argv);
void s21_getopt(struct flags *F, int argc, char **argv);

#endif