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

  /* Turn cos(4) into cos(4rad) if the angle unit is rad and cos(π) into
   * cos(π°) if the angle unit is deg, to notify the user of the current
   * angle unit she is using if she's forgetting to switch the angle unit */
  static void DeepAddAngleUnitToAmbiguousDirectFunctions(
      UserExpression& e, Preferences::AngleUnit angleUnit);
};

}  // namespace Poincare

#endif
