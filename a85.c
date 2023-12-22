#include "a85.h"
#include <assert.h>

void a85encode(char *buf, const uint8_t *data, size_t len) {
  assert(len % 4 == 0);
  static const char *a85 = "!\"#$%&'()*+,-./0"
                           "123456789:;<=>?@"
                           "ABCDEFGHIJKLMNOP"
                           "QRSTUVWXYZ[\\]^_`"
                           "abcdefghijklmnop"
                           "qrstu";

  uint32_t dividend = 0;
  for (size_t i = 0; i < len;) {
    dividend <<= 8;
    dividend += data[i++];

    if (i % 4)
      continue;

    if (dividend == 0) {
      *buf++ = 'z';
      dividend = 0;
      continue;
    }

    uint32_t divisor = 85 * 85 * 85 * 85;
    while (divisor) {
      *buf++ = a85[dividend / divisor % 85];
      divisor /= 85;
    }
    dividend = 0;
  }
  *buf = '\0';
}
