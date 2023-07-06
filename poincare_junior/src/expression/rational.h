#ifndef POINCARE_EXPRESSION_RATIONAL_H
#define POINCARE_EXPRESSION_RATIONAL_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "integer.h"

namespace PoincareJ {

class Rational final {
 public:
  static Tree* Push(IntegerHandler numerator, IntegerHandler denominator);
  static Tree* Push(const Tree* numerator, const Tree* denominator) {
    return Push(Integer::Handler(numerator), Integer::Handler(denominator));
  }
  static IntegerHandler Numerator(const Tree* node);
  static IntegerHandler Denominator(const Tree* node);
  static NonStrictSign Sign(const Tree* node) { return Numerator(node).sign(); }
  static StrictSign RationalStrictSign(const Tree* node) {
    return Numerator(node).strictSign();
  }
  static void SetSign(EditionReference reference, NonStrictSign sign);

  static Tree* Addition(const Tree* i, const Tree* j);
  static Tree* Multiplication(const Tree* i, const Tree* j);
  // IntegerPower of (p1/q1)^(p2) --> (p1^p2)/(q1^p2)
  static Tree* IntegerPower(const Tree* i, const Tree* j);
  static Tree* IrreducibleForm(const Tree* i);
};

}  // namespace PoincareJ

#endif
