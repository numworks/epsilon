#ifndef POINCARE_EXPRESSIONS_INTEGER_BIG_H
#define POINCARE_EXPRESSIONS_INTEGER_BIG_H

#include <poincare_junior/src/memory/type_block.h>
#include <poincare_junior/src/memory/value_block.h>

namespace Poincare {

class IntegerBig final {
  public:
  static const uint8_t * Digits(const TypeBlock * block) { return reinterpret_cast<const uint8_t *>(block->nextNth(2)); }
};

}

#endif


