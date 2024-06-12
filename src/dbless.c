#define _GNU_SOURCE // getpass
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "a85.h"
#include "sha256.h"

int main(int argc, char *argv[]) {
  struct SHA256Ctx ctx;
  sha256_init(&ctx);
  char **args = argv + 1;
  int args_len = argc - 1;
  char *master = getpass("Master password: ");

  sha256_update(&ctx, (uint8_t *)master, strlen(master));
  sha256_update(&ctx, (uint8_t *)"\n", 1);
  struct SHA256Ctx checksum_ctx = ctx;
  uint8_t *checksum = sha256_final(&checksum_ctx);

  for (int i = 0; i < args_len; i++) {
    sha256_update(&ctx, (uint8_t *)args[i], strlen(args[i]));
    sha256_update(&ctx, (uint8_t *)"\n", 1);
  }
  uint8_t *password = sha256_final(&ctx);

  fprintf(stderr, "Checksum: ");
  for (int i = 0; i < 8 / 2; i++)
    fprintf(stderr, "%02x", checksum[i]);
  fprintf(stderr, "\n");

  char buf[SHA256_HASH_SIZE * 5 / 4 + 1];
  a85_encode(buf, password, SHA256_HASH_SIZE);
  buf[SHA256_HASH_SIZE * 5 / 4] = '\0';
  fprintf(stdout, "%s", buf);

  return EXIT_SUCCESS;
}
