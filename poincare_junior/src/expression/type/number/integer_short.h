#ifndef POINCARE_EXPRESSIONS_NUMBER_INTEGER_SHORT_H
#define POINCARE_EXPRESSIONS_NUMBER_INTEGER_SHORT_H

#include <poincare_junior/src/memory/type_block.h>

namespace Poincare {

class IntegerShort final {
  /* | INTEGER_SHORT TAG | SIGNED DIGIT0 | INTEGER_SHORT TAG | */
public:
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, int8_t value) {
    switch (blockIndex) {
      case 0:
      case k_numberOfBlocksInNode - 1:
        *block = IntegerShortBlock;
        return blockIndex == k_numberOfBlocksInNode - 1;
      default:
        assert(blockIndex == 1);
        *block = ValueBlock(value);
        return false;
    }
  }
  constexpr static size_t k_numberOfBlocksInNode = 3;
  static int8_t Value(const TypeBlock * block) { return static_cast<int8_t>(*(block->next())); }
};

}

#endif
