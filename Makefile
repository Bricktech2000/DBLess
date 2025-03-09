CC=gcc
CFLAGS=-O2 -Wall -Wextra -Wpedantic -std=c99

all: bin/dbless

bin/dbless: src/dbless.c src/sha256.c src/a85.c src/sha256.h src/a85.h
	mkdir -p bin/
	$(CC) $(CFLAGS) -Wno-parentheses -Wno-unused-parameter $^ -o $@

clean:
	rm -rf bin/
