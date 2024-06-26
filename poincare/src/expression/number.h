#ifndef POINCARE_EXPRESSION_NUMBER_H
#define POINCARE_EXPRESSION_NUMBER_H

#include <poincare/src/memory/tree_ref.h>

#include "sign.h"

namespace Poincare::Internal {

// TODO: Pi and e have an odd status here, maybe Numbers should be split in two.

class Number {
 public:
  static bool IsStrictRational(const Tree* e) {
    return e->isOfType({Type::Half, Type::RationalNegShort,
                        Type::RationalPosShort, Type::RationalNegBig,
                        Type::RationalPosBig});
  }

  static Tree* Addition(const Tree* e1, const Tree* e2);
  static Tree* Multiplication(const Tree* e1, const Tree* e2);
  static Internal::Sign Sign(const Tree* e);
  static bool SetSign(Tree* e, NonStrictSign sign);
};

}  // namespace Poincare::Internal

#endif
