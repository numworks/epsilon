/* See the "Run-time ABI for the ARM Architecture", Section 4.2 */

typedef unsigned int uint32_t;

long long __aeabi_llsr(long long value, int shift) {
  uint32_t low = ((uint32_t)value >> shift);
  if (shift < 32) {
    low |= ((uint32_t)(value >> 32) << (32 - shift));
  } else {
    low |= ((uint32_t)(value >> 32) >> (shift - 32));
  }
  uint32_t high = (uint32_t)(value >> 32) >> shift;
  return ((long long)high << 32) | low;
}
