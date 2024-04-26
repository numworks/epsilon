#include <poincare/old/xnt_helpers.h>

#include <array>

#include "parsing/tokenizer.h"

namespace Poincare {

#define Layout OLayout

namespace XNTHelpers {

static int indexOfCodePointInCycle(CodePoint codePoint,
                                   const CodePoint* cycle) {
  if (codePoint == Symbol::k_radiusSymbol) {
    // r is not in the cycle, use θ instead
    codePoint = Symbol::k_polarSymbol;
  }
  for (size_t i = 0; i < k_maxCycleSize - 1; i++) {
    if (cycle[i] == codePoint) {
      return i;
    }
  }
  assert(cycle[k_maxCycleSize - 1] == codePoint);
  return k_maxCycleSize - 1;
}

static size_t sizeOfCycle(const CodePoint* cycle) {
  return indexOfCodePointInCycle(UCodePointNull, cycle);
}

static CodePoint codePointAtIndexInCycle(int index, int startingIndex,
                                         const CodePoint* cycle,
                                         size_t* cycleSize) {
  assert(index >= 0);
  assert(cycleSize);
  *cycleSize = sizeOfCycle(cycle);
  assert(0 <= startingIndex && startingIndex < static_cast<int>(*cycleSize));
  return cycle[(startingIndex + index) % *cycleSize];
}

CodePoint CodePointAtIndexInDefaultCycle(int index, CodePoint startingCodePoint,
                                         size_t* cycleSize) {
  int startingIndex =
      indexOfCodePointInCycle(startingCodePoint, k_defaultXNTCycle);
  return codePointAtIndexInCycle(index, startingIndex, k_defaultXNTCycle,
                                 cycleSize);
}

CodePoint CodePointAtIndexInCycle(int index, const CodePoint* cycle,
                                  size_t* cycleSize) {
  return codePointAtIndexInCycle(index, 0, cycle, cycleSize);
}

}  // namespace XNTHelpers

}  // namespace Poincare
