/* See the "Run-time ABI for the ARM Architecture", Section 4.2 */
#include <assert.h>

typedef unsigned int uint32_t;

long long __aeabi_llsr(long long value, int shift) {
  uint32_t low = ((uint32_t)value >> shift);
  /* "Shift behavior is undefined if the right operand is negative, or greater
   * than or equal to the length in bits of the promoted left operand" according
   * to C++ spec. However, arm compiler fill the vacated bits with 0 */
  assert(shift < 32 || low == 0);
  if (shift < 32) {
    low |= ((uint32_t)(value >> 32) << (32 - shift));
  } else {
    low |= ((uint32_t)(value >> 32) >> (shift - 32));
  }
  uint32_t high = (uint32_t)(value >> 32) >> shift;
  // Same comment
  assert(shift < 32 || high == 0);
  return ((long long)high << 32) | low;
}
