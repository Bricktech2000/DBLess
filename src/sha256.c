#include "sha256.h"
#include <assert.h>
#include <string.h>

#define ROTR(X, N) (X >> N | X << 32 - N)
#define SHR(X, N) (X >> N)

#define CH(X, Y, Z) (X & Y ^ ~X & Z)
#define MAJ(X, Y, Z) (X & Y ^ X & Z ^ Y & Z)
#define BSIGMA0(X) (ROTR(X, 2) ^ ROTR(X, 13) ^ ROTR(X, 22))
#define BSIGMA1(X) (ROTR(X, 6) ^ ROTR(X, 11) ^ ROTR(X, 25))
#define SSIGMA0(X) (ROTR(X, 7) ^ ROTR(X, 18) ^ SHR(X, 3))
#define SSIGMA1(X) (ROTR(X, 17) ^ ROTR(X, 19) ^ SHR(X, 10))

static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, //
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, //
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, //
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, //
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, //
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, //
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, //
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, //
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, //
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, //
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, //
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, //
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, //
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, //
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, //
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2, //
};

static const uint32_t H0[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, //
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19, //
};

void sha256_init(struct sha256_ctx *ctx) {
  ctx->nblocks = ctx->buflen = 0;
  memcpy(ctx->hash, H0, sizeof(H0));
}

void sha256_update(struct sha256_ctx *ctx, const uint8_t *data, size_t len) {
  while (1) {
    size_t n = SHA256_BLOCK_SIZE - ctx->buflen;
    memcpy(ctx->buf + ctx->buflen, data, n = len < n ? len : n);
    ctx->buflen += n, data += n, len -= n;
    if (ctx->buflen != SHA256_BLOCK_SIZE)
      break; // too little data to fill a block

    uint8_t *M = ctx->buf;
    uint32_t W[64];
    for (int s = 0, t = 0; t < 16; s += 4, t++)
      W[t] = M[s + 0] << 24 | M[s + 1] << 16 | M[s + 2] << 8 | M[s + 3] << 0;
    for (int t = 16; t < 64; t++)
      W[t] = SSIGMA1(W[t - 2]) + W[t - 7] + SSIGMA0(W[t - 15]) + W[t - 16];

    uint32_t *H = ctx->hash;
    uint32_t a = H[0], b = H[1], c = H[2], d = H[3];
    uint32_t e = H[4], f = H[5], g = H[6], h = H[7];

    for (int t = 0; t < 64; t++) {
      uint32_t T1 = h + BSIGMA1(e) + CH(e, f, g) + K[t] + W[t];
      uint32_t T2 = BSIGMA0(a) + MAJ(a, b, c);
      h = g, g = f, f = e, e = d + T1;
      d = c, c = b, b = a, a = T1 + T2;
    }

    H[0] += a, H[1] += b, H[2] += c, H[3] += d;
    H[4] += e, H[5] += f, H[6] += g, H[7] += h;

    ctx->nblocks++;
    ctx->buflen = 0;
  }
}

uint8_t *sha256_final(struct sha256_ctx *ctx) {
  uint64_t nbits = (ctx->nblocks * SHA256_BLOCK_SIZE + ctx->buflen) * 8;

  ctx->buf[ctx->buflen++] = 0x80; // append '1' bit
  int delta = SHA256_BLOCK_SIZE - ctx->buflen - 64 / 8;
  if (delta < 0)
    delta += SHA256_BLOCK_SIZE;
  static const uint8_t padding[SHA256_BLOCK_SIZE] = {0x00};
  sha256_update(ctx, padding, delta);

  for (int i = 0; i < 64 / 8; i++)
    ctx->buf[ctx->buflen++] = nbits >> (7 - i) * 8;
  sha256_update(ctx, NULL, 0);

  assert(ctx->buflen == 0); // assert multiple of 512 bits

  // use `ctx->buf` to store the final hash
  for (int i = 0, j = 0; i < 8; i++) {
    ctx->buf[j++] = ctx->hash[i] >> 24;
    ctx->buf[j++] = ctx->hash[i] >> 16;
    ctx->buf[j++] = ctx->hash[i] >> 8;
    ctx->buf[j++] = ctx->hash[i] >> 0;
  }

  return ctx->buf;
}
