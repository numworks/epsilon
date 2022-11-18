#ifndef POINCARE_EXPRESSIONS_NUMBERS_RATIONAL_SHORT_H
#define POINCARE_EXPRESSIONS_NUMBERS_RATIONAL_SHORT_H

namespace Poincare {

class RationalShort final {
public:
  static int8_t NumeratorValue(const TypeBlock * block) { return static_cast<int8_t>(*(block->next())); }
  static int8_t DenominatorValue(const TypeBlock * block) { return static_cast<int8_t>(*(block->nextNth(2))); }
};

}

#endif
