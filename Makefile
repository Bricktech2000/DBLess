build:
	mkdir -p bin
	gcc -O2 -Wall -Wno-parentheses -Werror -pedantic -std=c99 src/dbless.c src/sha256.c src/a85.c -o bin/dbless

clean:
	rm -rf bin
