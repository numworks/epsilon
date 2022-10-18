#ifndef POINCARE_EXPRESSIONS_RATIONAL_H
#define POINCARE_EXPRESSIONS_RATIONAL_H

#include "../../edition_reference.h"
#include "integer_handler.h"

namespace Poincare {

class Rational final {
public:
  static IntegerHandler Numerator(const TypeBlock * block);
  static IntegerHandler Denominator(const TypeBlock * block);
  static bool IsNegative(TypeBlock * block);
  static EditionReference Addition(Block * b1, Block * b2);
};

}

#endif
