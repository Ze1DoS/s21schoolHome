#ifndef GREP_OPTIONS_H
#define GREP_OPTIONS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <regex.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_FILENAMES 100
#define MAX_PATTERNS 100
#define MAX_LINE_LENGTH 1024

// Флаги
struct Options {
  int extendedPattern;
  int ignoreCase;
  int invertMatch;
  int countMatches;
  int listFiles;
  int lineNumbers;
  int hideFileNames;
  int silentMode;
  int filePattern;
  int onlyMatched;
  int noflag;
};

int grep(FILE *file, const char *filename, const char *pattern,
         struct Options *grep_options, int filename_count);

bool printLine(const char *line, int lineNumber, const char *filename,
               struct Options *grep_options, int fileCount, bool filenamePrinted);

int isRegularFile(const char *path);

void parseCommandLine(int argc, char *argv[], struct Options *grep_options);

int preparePatterns(int argc, char *argv[], char *patterns[], int patternCount,
                  struct Options *grep_options, char *regexPattern);

bool compileRegex(const char *pattern, regex_t *regex,
                  struct Options *grep_options);

int searchForPatternWithOflag(FILE *file, const char *filename, regex_t *regex,
                             struct Options *grep_options, int fileCount,
                             bool *filenamePrinted, bool *matchedInFile);

int searchForPattern(FILE *file, const char *filename, regex_t *regex,
                       struct Options *grep_options, int fileCount,
                       bool *filenamePrinted, bool *matchedInFile);

void printResult(struct Options *grep_options, const char *filename,
                 int fileCount, int count, bool matchedInFile);

#endif
