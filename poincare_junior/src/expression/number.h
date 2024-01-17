#ifndef POINCARE_EXPRESSION_NUMBER_H
#define POINCARE_EXPRESSION_NUMBER_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "sign.h"

namespace PoincareJ {

class Number {
 public:
  static bool IsStrictRational(const Tree* t) {
    return t->isOfType({BlockType::Half, BlockType::RationalShort,
                        BlockType::RationalNegBig, BlockType::RationalPosBig});
  }

  static Tree* Addition(const Tree* i, const Tree* j);
  static Tree* Multiplication(const Tree* i, const Tree* j);
  static Sign Sign(const Tree* node);
};

}  // namespace PoincareJ

#endif
