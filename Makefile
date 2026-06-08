.POSIX:
.SUFFIXES:
CC=gcc
CFLAGS=-O2 -Wall -Wextra -Wpedantic -std=c99

all: bin/dbless
bin/:; mkdir bin/
clean:; rm -rf bin/

bin/dbless: bin/ src/sha256.h src/a85.h src/dbless.c src/sha256.c src/a85.c; $(CC) $(CFLAGS) -o $@ src/dbless.c src/sha256.c src/a85.c -Wno-parentheses -Wno-unused-parameter
