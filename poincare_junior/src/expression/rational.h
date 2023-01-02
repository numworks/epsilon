#ifndef POINCARE_EXPRESSION_RATIONAL_H
#define POINCARE_EXPRESSION_RATIONAL_H

#include <poincare_junior/src/memory/edition_reference.h>
#include "integer.h"

namespace Poincare {

class Rational final {
public:
  static IntegerHandler Numerator(const Node node);
  static IntegerHandler Denominator(const Node node);
  static EditionReference PushNode(IntegerHandler numerator, IntegerHandler denominator);
  static NonStrictSign Sign(const Node node) { return Numerator(node).sign(); }
  static StrictSign StrictSign(const Node node) { return Numerator(node).strictSign(); }
  static void SetSign(EditionReference reference, NonStrictSign sign);

  // In-place??
  static EditionReference Addition(EditionReference n0, EditionReference n1);
};

}

#endif

