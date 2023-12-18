CC = gcc
CFLAGS = -Wall -Werror -Wextra 

OBJECTS = $(SOURCES:.c=.o)

all: s21_grep

s21_grep: s21_grep.c options.o
	$(CC) $(CFLAGS) -std=c11 s21_grep.c options.o -o s21_grep

options.o: options.c 
	$(CC) $(CFLAGS) -c options.c

n:
	clang-format -n *.c

i:
	clang-format -i *.c

clean:
	rm -f *.o