#ifndef POINCARE_EXPRESSION_RATIONAL_H
#define POINCARE_EXPRESSION_RATIONAL_H

#include <poincare_junior/src/memory/edition_reference.h>

#include "integer.h"

namespace PoincareJ {

class Rational final {
 public:
  static EditionReference Push(IntegerHandler numerator,
                               IntegerHandler denominator);
  static EditionReference Push(const Node numerator, const Node denominator) {
    return Push(Integer::Handler(numerator), Integer::Handler(denominator));
  }
  static IntegerHandler Numerator(const Node node);
  static IntegerHandler Denominator(const Node node);
  static NonStrictSign Sign(const Node node) { return Numerator(node).sign(); }
  static StrictSign RationalStrictSign(const Node node) {
    return Numerator(node).strictSign();
  }
  static void SetSign(EditionReference reference, NonStrictSign sign);

  static EditionReference Addition(const Node i, const Node j);
  static EditionReference Multiplication(const Node i, const Node j);
  // IntegerPower of (p1/q1)^(p2) --> (p1^p2)/(q1^p2)
  static EditionReference IntegerPower(const Node i, const Node j);
};

}  // namespace PoincareJ

#endif
