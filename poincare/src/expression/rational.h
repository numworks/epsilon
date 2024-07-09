#ifndef POINCARE_EXPRESSION_RATIONAL_H
#define POINCARE_EXPRESSION_RATIONAL_H

#include <poincare/src/memory/tree.h>

#include "integer.h"
#include "sign.h"

namespace Poincare::Internal {

/* Rationals should only exist in their irreducible form, this is ensured at
 * Rational Tree creation.
 * Since many form are forbidden, there could be room for a rational size
 * optimization. */

class Rational final {
 public:
  static Tree* Push(IntegerHandler numerator, IntegerHandler denominator);
  static Tree* Push(const Tree* numerator, const Tree* denominator) {
    return Push(Integer::Handler(numerator), Integer::Handler(denominator));
  }
  static IntegerHandler Numerator(const Tree* e);
  static IntegerHandler Denominator(const Tree* e);
  static Internal::Sign Sign(const Tree* e) {
    return Internal::Sign(e->isZero(), e->isStrictlyPositiveRational(),
                          e->isStrictlyNegativeRational(), !e->isInteger());
  }
  static bool SetSign(Tree* e, NonStrictSign sign);

  static Tree* Addition(const Tree* e1, const Tree* e2);
  static Tree* Multiplication(const Tree* e1, const Tree* e2);
  // IntegerPower of (p1/q1)^(p2) --> (p1^p2)/(q1^p2)
  static Tree* IntegerPower(const Tree* e1, const Tree* e2);

  static bool IsGreaterThanOne(const Tree* e);
  static Tree* CreateEuclideanDivision(const Tree* e);
  static Tree* CreateMixedFraction(const Tree* e,
                                   bool mixedFractionsAreEnabled);

 private:
  static bool IsIrreducible(const Tree* e);
  static Tree* PushIrreducible(IntegerHandler numerator,
                               IntegerHandler denominator);
  static Tree* PushIrreducible(const Tree* numerator, const Tree* denominator) {
    return PushIrreducible(Integer::Handler(numerator),
                           Integer::Handler(denominator));
  }
};

}  // namespace Poincare::Internal

#endif
