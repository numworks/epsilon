#ifndef POINCARE_TRIGONOMETRY_H
#define POINCARE_TRIGONOMETRY_H

#include <poincare/expression.h>

#include "evaluation.h"

namespace Poincare {

class Trigonometry final {
 public:
  enum class Function {
    Cosine = 0,
    Sine = 1,
  };
  static UserExpression PiExpressionInAngleUnit(
      Preferences::AngleUnit angleUnit);
  /* Returns a (unreduced) division between pi in each unit, or 1 if the units
   * are the same. */
  static UserExpression UnitConversionFactor(Preferences::AngleUnit fromUnit,
                                             Preferences::AngleUnit toUnit);
#if 0
  // TODO_PCJ: Delete these method
  static Expression ReplaceWithAdvancedFunction(Expression& e,
                                                Expression& denominator);
#endif
};

}  // namespace Poincare

#endif
