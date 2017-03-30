/* See the "Run-time ABI for the ARM Architecture", Section 4.4.5 */

long long __aeabi_llsr(long long value, int shift) {
  int low = ((int)value >> shift) | ((int)(value >> 32) << (32 - shift));
  int high = (int)value >> shift;
  return ((long long)high << 32) | low;
}
