/* See the "Run-time ABI for the ARM Architecture", Section 4.2 */

typedef unsigned int uint32_t;

long long __aeabi_llsl(long long value, int shift) {
  uint32_t low = (uint32_t)value << shift;
  uint32_t high = ((uint32_t)(value >> 32) << shift);
  if (shift < 32) {
    high |= ((uint32_t)value >> (32 - shift));
  } else {
    high |= ((uint32_t)value << (shift - 32));
  }
  return ((long long)high << 32) | low;
}
