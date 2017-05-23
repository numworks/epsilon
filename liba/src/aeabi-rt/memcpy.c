#include <stddef.h>
#include <string.h>

/* See the "Run-time ABI for the ARM Architecture", Section 4.3.4 */

void __aeabi_memcpy(void * dest, const void * src, size_t n) {
  memcpy(dest, src, n);
}

// TODO: optimize aeabi_memcpy4 to take advantage of the 4-byte alignment
void __aeabi_memcpy4(void * dest, const void * src, size_t n) {
  memcpy(dest, src, n);
}
