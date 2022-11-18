#ifndef POINCARE_EXPRESSIONS_NUMBER_INTEGER_SHORT_H
#define POINCARE_EXPRESSIONS_NUMBER_INTEGER_SHORT_H

#include <poincare_junior/src/memory/type_block.h>

namespace Poincare {

class IntegerShort final {
public:
  static int8_t Value(const TypeBlock * block) { return static_cast<int8_t>(*(block->next())); }
};

}

#endif
