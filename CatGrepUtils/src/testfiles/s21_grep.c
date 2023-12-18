#include "options.h"

int main(int argc, char **argv) {
  if (argc < 3) return 0;
  argv++, argc--;
  struct flags F = {0};
  s21_getopt(&F, argc, argv);
  return 0;
}