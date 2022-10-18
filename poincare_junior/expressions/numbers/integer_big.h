#ifndef POINCARE_EXPRESSIONS_INTEGER_BIG_H
#define POINCARE_EXPRESSIONS_INTEGER_BIG_H

#include "../../type_block.h"
#include "../../value_block.h"

namespace Poincare {

class IntegerBig final {
  /* | INTEGER_BIG TAG | NUMBER DIGITS | UNSIGNED DIGIT0 | ... | NUMBER DIGITS | INTEGER_BIG TAG | */
public:
  constexpr static size_t k_numberOfMetaBlocksInNode = 4;
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, int value) {
    if (blockIndex == 0) {
      bool neg = value < 0;
      *block = neg ? IntegerNegBigBlock : IntegerPosBigBlock;
      return false;
    }
    int absValue = std::abs(value);
    uint8_t numberOfDigits = 0;
    while (absValue && numberOfDigits < 4) {
      absValue = value >> 8;
      numberOfDigits++;
    }
    if (blockIndex == k_numberOfMetaBlocksInNode + numberOfDigits - 1) {
      CreateBlockAtIndex(block, 0, value);
      return true;
    }
    if (blockIndex == 1 || blockIndex == k_numberOfMetaBlocksInNode + numberOfDigits - 2) {
      *block = ValueBlock(numberOfDigits);
      return false;
    }
    int maskOffset = (blockIndex - 2) * 8;
    assert(maskOffset <= sizeof(int)/sizeof(uint8_t) * 8);
    *block = ValueBlock(absValue & (0xFF << maskOffset));
    return false;
  }

  static const uint8_t * Digits(const TypeBlock * block) { return reinterpret_cast<const uint8_t *>(block->nextNth(2)); }
  constexpr static uint8_t NumberOfDigits(const TypeBlock * block, bool head = true) { return static_cast<uint8_t>(*(head ? block->next() : block->previous())); }
};

}

#endif


