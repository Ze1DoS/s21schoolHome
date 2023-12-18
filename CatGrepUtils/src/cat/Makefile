all: s21_cat

s21_cat: catting.c table.o
	gcc -Wall -Wextra -Werror catting.c table.o -o s21_cat

table.o: table.c
	gcc -c table.c