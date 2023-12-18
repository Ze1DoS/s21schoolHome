CC = gcc
CFLAGS = -Wall -Werror -Wextra 

OBJECTS = $(SOURCES:.c=.o)

all: s21_grep clean format

s21_grep: grep_main.c grep_options.o
	$(CC) $(CFLAGS) -std=c11 grep_main.c grep_options.o -o s21_grep

grep_options.o: grep_options.c 
	$(CC) $(CFLAGS) -c grep_options.c

clean:
	rm -f *.o

format:
	clang-format -i *.c
