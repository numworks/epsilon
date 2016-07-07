#include <stddef.h>
#include <string.h>

/* See the "Run-time ABI for the ARM Architecture", Section 4.3.3 */

void __aeabi_memcpy(void * dest, const void * src, size_t n) {
  memcpy(dest, src, n);
}
