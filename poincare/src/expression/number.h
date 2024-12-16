#ifndef POINCARE_EXPRESSION_NUMBER_H
#define POINCARE_EXPRESSION_NUMBER_H

#include <poincare/src/expression/sign.h>
#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

// TODO: Pi and e have an odd status here, maybe Numbers should be split in two.

class Number {
 public:
  // Return true if e is zero or a null float.
  static bool IsNull(const Tree* e);
  // Return true if e is one or a float 1.
  static bool IsOne(const Tree* e);
  static bool IsStrictRational(const Tree* e) {
    return e->isOfType({Type::Half, Type::RationalNegShort,
                        Type::RationalPosShort, Type::RationalNegBig,
                        Type::RationalPosBig});
  }
  static Tree* Addition(const Tree* e1, const Tree* e2);
  static Tree* Multiplication(const Tree* e1, const Tree* e2);
  static Poincare::Sign Sign(const Tree* e);
  static bool SetSign(Tree* e, NonStrictSign sign);
};

}  // namespace Poincare::Internal

#endif
