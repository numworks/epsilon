#ifndef POINCARE_EXPRESSIONS_NUMBERS_RATIONAL_SHORT_H
#define POINCARE_EXPRESSIONS_NUMBERS_RATIONAL_SHORT_H

namespace Poincare {

class RationalShort final {
  /* | RATIONAL_SHORT TAG | SIGNED DIGIT | UNSIGNED DIGIT | RATIONAL_SHORT TAG | */
public:
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, int8_t numerator, uint8_t denominator) {
    switch (blockIndex) {
      case 0:
      case k_numberOfBlocksInNode - 1:
        *block = RationalShortBlock;
        return blockIndex == k_numberOfBlocksInNode - 1;
      case 1:
        *block = ValueBlock(numerator);
        return false;
      default:
        assert(blockIndex == 2);
        *block = ValueBlock(denominator);
        return false;
    }
  }
  constexpr static size_t k_numberOfBlocksInNode = 4;
  static int8_t NumeratorValue(const TypeBlock * block) { return static_cast<int8_t>(*(block->next())); }
  static int8_t DenominatorValue(const TypeBlock * block) { return static_cast<int8_t>(*(block->nextNth(2))); }
};

}

#endif
