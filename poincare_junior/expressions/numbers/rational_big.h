#ifndef POINCARE_EXPRESSIONS_NUMBERS_RATIONAL_BIG_H
#define POINCARE_EXPRESSIONS_NUMBERS_RATIONAL_BIG_H

namespace Poincare {

class RationalBig final {
public:
  constexpr static size_t k_numberOfMetaBlocksInNode = 5;
  static const uint8_t * NumeratorDigits(const TypeBlock * block) { return reinterpret_cast<const uint8_t *>(block->nextNth(3)); }
  static const uint8_t * DenominatorDigits(const TypeBlock * block) { return reinterpret_cast<const uint8_t *>(block->nextNth(3 + NumeratorNumberOfDigits(block))); }
  constexpr static uint8_t NumeratorNumberOfDigits(const TypeBlock * block) { return static_cast<uint8_t>(*(block->next())); }
  constexpr static uint8_t DenominatorNumberOfDigits(const TypeBlock * block) { return static_cast<uint8_t>(*(block->nextNth(2))); }
  constexpr static uint8_t NumberOfDigitsFromTail(const TypeBlock * block) { return static_cast<uint8_t>(*(block->previous())); }
};

}

#endif
