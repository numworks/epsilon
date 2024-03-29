#ifndef POINCARE_EXPRESSION_RATIONAL_H
#define POINCARE_EXPRESSION_RATIONAL_H

#include <omg/enums.h>
#include <poincare_junior/src/memory/tree.h>

#include "integer.h"
#include "sign.h"

namespace PoincareJ {

class Rational final {
 public:
  static Tree* Push(IntegerHandler numerator, IntegerHandler denominator);
  static Tree* Push(const Tree* numerator, const Tree* denominator) {
    return Push(Integer::Handler(numerator), Integer::Handler(denominator));
  }
  static IntegerHandler Numerator(const Tree* node);
  static IntegerHandler Denominator(const Tree* node);
  static PoincareJ::Sign Sign(const Tree* node) {
    StrictSign s = Numerator(node).strictSign();
    return PoincareJ::Sign(s == StrictSign::Null, s == StrictSign::Positive,
                           s == StrictSign::Negative, !node->isInteger());
  }
  static bool SetSign(Tree* reference, NonStrictSign sign);

  static Tree* Addition(const Tree* i, const Tree* j);
  static Tree* Multiplication(const Tree* i, const Tree* j);
  // IntegerPower of (p1/q1)^(p2) --> (p1^p2)/(q1^p2)
  static Tree* IntegerPower(const Tree* i, const Tree* j);

  static bool IsGreaterThanOne(const Tree* r);
  static Tree* CreateMixedFraction(const Tree* r,
                                   bool mixedFractionsAreEnabled);

 private:
  static bool IsIrreducible(const Tree* i);
  static Tree* PushIrreducible(IntegerHandler numerator,
                               IntegerHandler denominator);
  static Tree* PushIrreducible(const Tree* numerator, const Tree* denominator) {
    return PushIrreducible(Integer::Handler(numerator),
                           Integer::Handler(denominator));
  }
};

}  // namespace PoincareJ

#endif
