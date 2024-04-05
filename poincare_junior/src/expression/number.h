#ifndef POINCARE_EXPRESSION_NUMBER_H
#define POINCARE_EXPRESSION_NUMBER_H

#include <omg/enums.h>
#include <poincare_junior/src/memory/edition_reference.h>

#include "sign.h"

namespace PoincareJ {

// TODO: Pi and e have an odd status here, maybe Numbers should be split in two.

class Number {
 public:
  static bool IsStrictRational(const Tree* t) {
    return t->isOfType({Type::Half, Type::RationalShort, Type::RationalNegBig,
                        Type::RationalPosBig});
  }

  static Tree* Addition(const Tree* i, const Tree* j);
  static Tree* Multiplication(const Tree* i, const Tree* j);
  static PoincareJ::Sign Sign(const Tree* node);
  static bool SetSign(Tree* number, NonStrictSign sign);
};

}  // namespace PoincareJ

#endif
