#ifndef POINCARE_EXPRESSIONS_INTEGER_H
#define POINCARE_EXPRESSIONS_INTEGER_H

#include "integer_big.h"
#include "integer_handler.h"
#include "integer_short.h"
#include "../../edition_reference.h"

namespace Poincare {

class Integer {
public:
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, int value) {
    if (value < Block::k_maxValue) {
      return IntegerShort::CreateBlockAtIndex(block, blockIndex, static_cast<int8_t>(value));
    } else {
      return IntegerBig::CreateBlockAtIndex(block, blockIndex, value);
    }
  }
  static EditionReference Addition(IntegerHandler a, IntegerHandler b);
};


}

#endif

