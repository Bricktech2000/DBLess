#define _POSIX_SOURCE // fileno
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include "a85.h"
#include "sha256.h"

// Android's `getpass` writes the prompt and forced newline to `stdout` whereas
// GNU's `getpass` writes them either to `/dev/tty` or to `stderr`. this manual
// implementation dodges the mess so users can pipe stdio confidently
char *getpass(FILE *stream) {
  // on failure, returns `NULL` and sets `errno`

  struct termios orig;
  if (tcgetattr(fileno(stream), &orig) != 0)
    return NULL;

  struct termios term = orig;
  term.c_lflag &= ~ECHO;
  term.c_lflag |= ECHONL;

  if (tcsetattr(fileno(stream), TCSAFLUSH, &term) != 0)
    return NULL;

  size_t len = 0, cap = 256;
  char *nl, *line = malloc(cap);
  while (fgets(line + len, cap - len, stream) != NULL) {
    if ((nl = memchr(line + len, '\n', cap - len)) == NULL) {
      len = cap - 1, line = realloc(line, cap *= 2);
      continue;
    }
    *nl = '\0', len = 0;

    if (tcsetattr(fileno(stream), TCSAFLUSH, &orig) != 0)
      return NULL;

    return line;
  }

  if (feof(stdin))
    errno = ECANCELED; // EOF hit before newline found
  return NULL;
}

int main(int argc, char *argv[]) {
  struct sha256_ctx ctx;
  sha256_init(&ctx);
  fprintf(stderr, "Master password: ");
  char *master = getpass(stdin);
  if (master == NULL)
    perror("getpass"), exit(EXIT_FAILURE);

  sha256_update(&ctx, (uint8_t *)master, strlen(master));
  sha256_update(&ctx, (uint8_t *)"\n", 1);
  memset(master, '\0', strlen(master)), free(master);
  struct sha256_ctx checksum_ctx = ctx;
  uint8_t *checksum = sha256_final(&checksum_ctx);

  for (char **arg = argv + 1; *arg; arg++) {
    sha256_update(&ctx, (uint8_t *)*arg, strlen(*arg));
    sha256_update(&ctx, (uint8_t *)"\n", 1);
  }
  uint8_t *password = sha256_final(&ctx);

  fprintf(stderr, "Checksum: ");
  for (int i = 0; i < 8 / 2; i++)
    fprintf(stderr, "%02x", checksum[i]);
  fprintf(stderr, "\n");

  char buf[SHA256_HASH_SIZE * 5 / 4 + 1];
  assert(a85_encode(buf, password, SHA256_HASH_SIZE));
  buf[SHA256_HASH_SIZE * 5 / 4] = '\0';
  fprintf(stdout, "%s", buf);
}
