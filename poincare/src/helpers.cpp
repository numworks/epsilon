#include <poincare/helpers.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

namespace Helpers {

size_t AlignedSize(size_t realSize, size_t alignment) {
  size_t modulo = realSize % alignment;
  size_t result = realSize + (modulo == 0 ? 0 : alignment - modulo);
  assert(result % alignment == 0);
  return result;
}

size_t Gcd(size_t a, size_t b) {
  int i = a;
  int j = b;
  do {
    if (i == 0) {
      return j;
    }
    if (j == 0) {
      return i;
    }
    if (i > j) {
      i = i - j*(i/j);
    } else {
      j = j - i*(j/i);
    }
  } while(true);
}


bool Rotate(uint32_t * dst, uint32_t * src, size_t len) {
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

  size_t numberOfCycles = Gcd(len, dst < src ? src - dst : dst - src);
  size_t dstAddressOffset = dst < src ? len : dst - len - src;

  // We need the limit addresses of the data that will change
  uint32_t * insertionZoneStart = dst < src ? dst : src;
  uint32_t * insertionZoneEnd = dst < src ? src + len - 1 : dst - 1;

  uint32_t * cycleStartAddress;
  uint32_t * moveSrcAddress;
  uint32_t * moveDstAddress;
  uint32_t tmpData;
  uint32_t nextTmpData;

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

void Sort(Swap swap, Compare compare, void * context, int numberOfElements) {
  /* Using an insertion-sort algorithm, which has the advantage of being
   * in-place and efficient when already sorted. It is optimal if Compare is
   * more lenient with equalities ( >= instead of > ) */
  for (int i = 1; i < numberOfElements; i++) {
    for (int j = i - 1; j >= 0; j--) {
      if (compare(j+1, j, context, numberOfElements)) {
        break;
      }
      swap(j, j+1, context, numberOfElements);
    }
  }
}

int ExtremumIndex(Compare compare, void * context, int numberOfElements, bool minimum) {
  int returnIndex = 0;
  for (int i = 0; i < numberOfElements; i++) {
    bool newIsGreater = compare(i, returnIndex, context, numberOfElements);
    if ((minimum && !newIsGreater) || (!minimum && newIsGreater)) {
      returnIndex = i;
    }
  }
  return returnIndex;
}

bool FloatIsGreater(float xI, float xJ, bool nanIsGreatest) {
  if (std::isnan(xI)) {
    return nanIsGreatest;
  }
  if (std::isnan(xJ)) {
    return !nanIsGreatest;
  }
  return xI > xJ;
}

}

}
