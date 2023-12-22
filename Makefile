build:
	mkdir -p bin
	gcc -O2 -Wall -Werror -pedantic -std=c99 dbless.c sha256.c a85.c -o bin/dbless

clean:
	rm -rf bin
