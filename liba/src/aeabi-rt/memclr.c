#include <stddef.h>
#include <string.h>

/* See the "Run-time ABI for the ARM Architecture", Section 4.3.4 */

void __aeabi_memclr(void * dest, size_t n) {
  memset(dest, 0, n);
}

void __aeabi_memclr4(void * dest, size_t n) {
  memset(dest, 0, n);
}

void __aeabi_memclr8(void * dest, size_t n) {
  memset(dest, 0, n);
}
