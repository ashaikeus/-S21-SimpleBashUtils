all: s21_cat s21_grep

s21_cat:
	gcc -Wall -Werror -Wextra -std=c11 ./cat/s21_cat.c -o ./cat/s21_cat

s21_grep:
	gcc -Wall -Werror -Wextra -std=c11 ./grep/s21_grep.c -o ./grep/s21_grep

clean:
	rm ./cat/s21_cat
	rm ./grep/s21_grep

rebuild: clean all