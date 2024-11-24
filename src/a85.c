#include "a85.h"
#include <string.h>

static const char *a85 = "!\"#$%&'()*+,-./0"
                         "123456789:;<=>?@"
                         "ABCDEFGHIJKLMNOP"
                         "QRSTUVWXYZ[\\]^_`"
                         "abcdefghijklmnop"
                         "qrstu";

// these routines are no fancier than they need to be. this means:
// - `"    "` does not encode to shorthand `'y'`
// - `"\0\0\0\0"` does not encode to shorthand `'z'`
// - shorthand `'y'` does not decode to `"    "`
// - shorthand `'z'` does not decode to `"\0\0\0\0"`
// - encoder does not pad input to multiple of 4
// - decoder does not pad input to multiple of 5
// - decoder does not ignore whitespace characters

char *a85_encode(char *buf, const uint8_t *data, size_t len) {
  // returns a pointer one past the last `char` written to `buf`, or `NULL` on
  // encode error. does not null-terminate `buf`

  if (len % 4)
    return NULL;

  uint32_t dividend = 0;
  for (size_t i = 0; i < len;) {
    dividend <<= 8, dividend |= data[i++];

    if (i % 4)
      continue;

    for (uint32_t divisor = 85 * 85 * 85 * 85; divisor; divisor /= 85)
      *buf++ = a85[dividend / divisor % 85];
    dividend = 0;
  }

  return buf;
}

uint8_t *a85_decode(uint8_t *buf, const char *data, size_t len) {
  // returns a pointer one past the last `uint8_t` written to `buf`, or `NULL`
  // on decode error. `data` need not be null terminated

  if (len % 5)
    return NULL;

  uint32_t dividend = 0;
  for (size_t i = 0; i < len;) {
    char *idx = strchr(a85, data[i++]);
    if (idx == NULL)
      return NULL;

    if (dividend > UINT32_MAX / 85)
      return NULL;
    dividend *= 85;

    if (dividend > UINT32_MAX - (idx - a85))
      return NULL;
    dividend += idx - a85;

    if (i % 5)
      continue;

    for (int shift = 8 + 8 + 8; shift >= 0; shift -= 8)
      *buf++ = dividend >> shift & 0xff;
    dividend = 0;
  }

  return buf;
}
