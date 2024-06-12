#include "a85.h"
#include <assert.h>
#include <stddef.h>

const char *a85 = "!\"#$%&'()*+,-./0"
                  "123456789:;<=>?@"
                  "ABCDEFGHIJKLMNOP"
                  "QRSTUVWXYZ[\\]^_`"
                  "abcdefghijklmnop"
                  "qrstu";

void a85_encode(char *buf, const uint8_t *data, size_t len) {
  assert(len % 4 == 0);

  uint32_t dividend = 0;
  for (size_t i = 0; i < len;) {
    dividend <<= 8, dividend |= data[i++];

    if (i % 4)
      continue;

    if (dividend == 0) {
      *buf++ = 'z';
      continue;
    }

    uint32_t divisor = 85 * 85 * 85 * 85;
    for (; divisor; divisor /= 85)
      *buf++ = a85[dividend / divisor % 85];
    dividend = 0;
  }

  *buf = '\0';
}
