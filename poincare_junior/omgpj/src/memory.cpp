#include <omgpj/arithmetic.h>
#include <omgpj/memory.h>
#include <assert.h>

size_t Memory::AlignedSize(size_t realSize, size_t alignment) {
  size_t modulo = realSize % alignment;
  size_t result = realSize + (modulo == 0 ? 0 : alignment - modulo);
  assert(result % alignment == 0);
  return result;
}

bool Memory::Rotate(uint8_t * dst, uint8_t * src, size_t len) {
  /* This method "rotates" an array to insert data at src with length len at
   * address dst.
   *
   * For instance, rotate(2, 4, 1) is:
   *
   * | 0 | 1 | 2 | 3 | 4 | 5 | 6 |
   *         ^¨¨¨¨¨¨¨^---^
   *        Dst       Src       Len = 1
   *
   * After rotation :
   * | 0 | 1 | 4 | 2 | 3 | 5 | 6 |
   *          --- ¨¨¨¨¨¨¨
   */

  if (len == 0 || src == dst || (dst > src && dst < src + len)) {
    return false;
  }

  /* We start with the first data to move at address a0 (we chose src but this
   * does not matter), move it to its final address (a1 = a0 + len). We then
   * move the data that was in a1 to its final address (a2) and so on, until we
   * set the data at the starting address a0.
   * This is a cycle of changes, and we have to make GCD(len, |dst-src]) such
   * cycles, each starting at the previous cycle starting address + 1.
   * This is a one-move per data algorithm, so it is O(dst - src) if dst > src,
   * else O(src+len - dst). */

  size_t numberOfCycles = Arithmetic::Gcd(len, dst < src ? src - dst : dst - src);
  size_t dstAddressOffset = dst < src ? len : dst - len - src;

  // We need the limit addresses of the data that will change
  uint8_t * insertionZoneStart = dst < src ? dst : src;
  uint8_t * insertionZoneEnd = dst < src ? src + len - 1 : dst - 1;

  uint8_t * cycleStartAddress;
  uint8_t * moveSrcAddress;
  uint8_t * moveDstAddress;
  uint8_t tmpData;
  uint8_t nextTmpData;

  for (size_t i = 0; i < numberOfCycles; i++) {
    // Set the cycle starting source
    cycleStartAddress = src + i;
    assert(cycleStartAddress <= insertionZoneEnd);
    moveSrcAddress = cycleStartAddress;
    // Set the cycle starting destination, dstAddressOffset after the source
    moveDstAddress = moveSrcAddress + dstAddressOffset;
    if (moveDstAddress > insertionZoneEnd) {
      moveDstAddress = insertionZoneStart + (moveDstAddress - insertionZoneEnd - 1);
    }
    tmpData = *moveSrcAddress;
    do {
      nextTmpData = *moveDstAddress;
      *moveDstAddress = tmpData;
      tmpData = nextTmpData;
      moveSrcAddress = moveDstAddress;
      moveDstAddress = moveSrcAddress + dstAddressOffset;
      if (moveDstAddress > insertionZoneEnd) {
        moveDstAddress = insertionZoneStart + (moveDstAddress - insertionZoneEnd - 1);
      }
    } while (moveSrcAddress != cycleStartAddress);
  }
  return true;
}
