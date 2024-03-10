#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "a85.h"
#include "sha256.h"

extern char *getpass(const char *prompt); // from unistd.h

int main(int argc, char *argv[]) {
  int args_len = argc - 1;
  char **args = argv + 1;
  BYTE digest[SHA256_BLOCK_SIZE];
  BYTE checksum[SHA256_BLOCK_SIZE];
  SHA256_CTX ctx;
  sha256_init(&ctx);

  char *master = getpass("Master password: ");

  sha256_update(&ctx, (const BYTE *)master, strlen(master));
  SHA256_CTX ctx_copy = ctx;
  sha256_final(&ctx_copy, checksum);
  for (int i = 0; i < args_len; i++)
    sha256_update(&ctx, (const BYTE *)args[i], strlen(args[i]));
  sha256_final(&ctx, digest);

  fprintf(stderr, "Checksum: ");
  for (int i = 0; i < 8 / 2; i++)
    fprintf(stderr, "%02x", checksum[i]);
  fprintf(stderr, "\n");

  char buf[SHA256_BLOCK_SIZE * 5 / 4 + 1];
  a85encode(buf, digest, SHA256_BLOCK_SIZE);
  fprintf(stdout, "%s", buf);

  return EXIT_SUCCESS;
}
