CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Werror -g
TEST_FLAGS=-lcheck -lpthread -lrt -lsubunit -lm -g
GCOV_FLAGS=-coverage -Wall -Wextra -Werror

SOURSES=$(wildcard *.c)
OBJECTS=$(SOURSES:.c=.o)
TEST_SOURSES=$(wildcard ./tests/*.c)
TEST_OBJECTS=$(TEST_SOURSES:.c=.o)

all: clean s21_decimal.a

test: unittests.c s21_decimal.a
	$(CC) ./tests/unittests.o s21_decimal.a -o test $(TEST_FLAGS)
	./test

unittests.c:
	$(CC) $(CFLAGS) -c tests/unittests.c -o tests/unittests.o 

s21_decimal.a: $(OBJECTS)
	ar -rcs s21_decimal.a $(OBJECTS)
	rm -rf *.o

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./test

# ВНЕ кампуса
# gcov_report: $(TEST_OBJECTS)
# 	$(CC) $(GCOV_FLAGS) $(TEST_OBJECTS) $(SOURSES) $(TEST_FLAGS) -o test
# 	./test
# 	gcov *.gcno
# 	lcov -t "test" -o test.info -c -d . --rc lcov_branch_coverage=1
# 	genhtml -o report test.info --rc lcov_branch_coverage=1
# 	open report/index.html

# ДЛЯ кампуса
gcov_report: $(TEST_OBJECTS)
	$(CC) --coverage $(GCOV_FLAGS) $(TEST_OBJECTS) $(SOURSES) $(TEST_FLAGS) -o test
	./test
	~/.local/bin/gcovr -r . --html-details -o gcov_report.html
	mkdir gcov_report
	mv *.html *.css gcov_report
	rm -rf *.gcno *.gcda
	open gcov_report/gcov_report.html

style:
	cp ../materials/linters/.clang-format .
	clang-format -n *.c *.h tests/*.c
# clang-format -i *.c *.h tests/*.c
	rm .clang-format

clean:
	rm -f s21_decimal.a test test.info
	rm -rf *.html *.css gcov_report
	find . -type f -name '*.o' -delete
	find . -type f -name '*.gcda' -delete
	find . -type f -name '*.gcno' -delete
	find . -type f -name '*.gcov' -delete
	rm -fr report/
