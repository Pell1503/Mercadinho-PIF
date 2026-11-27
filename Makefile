CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

pdv: main.c
	$(CC) $(CFLAGS) -o pdv main.c

test: pdv
	./tests/run_tests.sh

clean:
	rm -f pdv pdv.exe

.PHONY: test clean
