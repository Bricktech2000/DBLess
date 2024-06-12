#include <stddef.h>
#include <stdint.h>

#define SHA256_BLOCK_SIZE (512 / 8)
#define SHA256_HASH_SIZE (256 / 8)

struct SHA256Ctx {
  uint64_t nblocks;
  uint32_t hash[8];
  uint8_t buflen;
  uint8_t buf[SHA256_BLOCK_SIZE];
};

void sha256_init(struct SHA256Ctx *ctx);
void sha256_update(struct SHA256Ctx *ctx, const uint8_t *data, size_t len);
uint8_t *sha256_final(struct SHA256Ctx *ctx);
