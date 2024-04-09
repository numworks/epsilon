#ifndef POINCARE_EXPRESSION_NUMBER_H
#define POINCARE_EXPRESSION_NUMBER_H

#include <poincare/src/memory/tree_ref.h>

#include "sign.h"

namespace Poincare::Internal {

// TODO: Pi and e have an odd status here, maybe Numbers should be split in two.

class Number {
 public:
  static bool IsStrictRational(const Tree* t) {
    return t->isOfType({Type::Half, Type::RationalShort, Type::RationalNegBig,
                        Type::RationalPosBig});
  }

  static Tree* Addition(const Tree* i, const Tree* j);
  static Tree* Multiplication(const Tree* i, const Tree* j);
  static Internal::Sign Sign(const Tree* node);
  static bool SetSign(Tree* number, NonStrictSign sign);
};

}  // namespace Poincare::Internal

#endif
