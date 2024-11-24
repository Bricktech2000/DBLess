#include <stddef.h>
#include <stdint.h>

char *a85_encode(char *buf, const uint8_t *data, size_t len);
uint8_t *a85_decode(uint8_t *buf, const char *data, size_t len);
