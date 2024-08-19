.PHONY: all test clean format
.POSIX:

CC=gcc

build:
	$(CC) main.c -o main
	$(CC) fun.c -c

format:
	astyle -q -Z -n -A3 -t8 -p -xg -H -j -xB *.[ch]

clean:
	rm -f test
