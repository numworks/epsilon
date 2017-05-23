/* See the "Run-time ABI for the ARM Architecture", Section 4.2 */

typedef int uint32_t;

long long __aeabi_llsl(long long value, int shift) {
  uint32_t low = (uint32_t)value << shift;
  uint32_t high = ((uint32_t)(value >> 32) << shift) | ((uint32_t)value >> (32 - shift));
  return ((long long)high << 32) | low;
}
