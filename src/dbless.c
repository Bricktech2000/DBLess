#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "a85.h"
#include "sha256.h"

extern char *getpass(const char *prompt); // from unistd.h

int main(int argc, char *argv[]) {
  SHA256_CTX ctx;
  sha256_init(&ctx);
  char **args = argv + 1;
  int args_len = argc - 1;
  char *master = getpass("Master password: ");

  BYTE checksum[SHA256_BLOCK_SIZE];
  sha256_update(&ctx, (BYTE *)master, strlen(master));
  sha256_update(&ctx, (BYTE *)"\n", 1);
  SHA256_CTX ctx_copy = ctx;
  sha256_final(&ctx_copy, checksum);

  BYTE digest[SHA256_BLOCK_SIZE];
  for (int i = 0; i < args_len; i++) {
    sha256_update(&ctx, (BYTE *)args[i], strlen(args[i]));
    sha256_update(&ctx, (BYTE *)"\n", 1);
  }
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
