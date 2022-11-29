#ifndef POINCARE_EXPRESSION_RATIONAL_H
#define POINCARE_EXPRESSION_RATIONAL_H

#include <poincare_junior/src/memory/edition_reference.h>
#include "integer.h"

namespace Poincare {

class Rational final {
public:
  static IntegerHandler Numerator(const Node node);
  static IntegerHandler Denominator(const Node node);

  static EditionReference Addition(Node n0, Node n1);
};

}

#endif

