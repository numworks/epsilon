#ifndef POINCARE_EXPRESSION_RATIONAL_H
#define POINCARE_EXPRESSION_RATIONAL_H

#include <poincare_junior/src/memory/edition_reference.h>
#include "integer.h"

namespace Poincare {

class Rational final {
public:
  static IntegerHandler Numerator(const TypeBlock * block);
  static IntegerHandler Denominator(const TypeBlock * block);

  static EditionReference Addition(Block * b1, Block * b2);
};

}

#endif

