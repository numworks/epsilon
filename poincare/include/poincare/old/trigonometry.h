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
  static UserExpression AnglePeriodInAngleUnit(
      Preferences::AngleUnit angleUnit);
  static UserExpression PiExpressionInAngleUnit(
      Preferences::AngleUnit angleUnit);
  static bool IsDirectTrigonometryFunction(const UserExpression& e);
  static bool IsInverseTrigonometryFunction(const UserExpression& e);
  static bool IsAdvancedTrigonometryFunction(const UserExpression& e);
  static bool IsInverseAdvancedTrigonometryFunction(const UserExpression& e);
  static bool AreInverseFunctions(const UserExpression& directFunction,
                                  const UserExpression& inverseFunction);
  /* Returns a (unreduced) division between pi in each unit, or 1 if the units
   * are the same. */
  static UserExpression UnitConversionFactor(Preferences::AngleUnit fromUnit,
                                             Preferences::AngleUnit toUnit);
  static bool ExpressionIsEquivalentToTangent(const UserExpression& e);
  static bool ExpressionIsEquivalentToInverseOfTangent(const UserExpression& e);
  // TODO_PCJ: Delete these method
#if 0
  static Expression ShallowReduceDirectFunction(
      Expression& e, ReductionContext reductionContext);
  static Expression ShallowReduceInverseFunction(
      Expression& e, ReductionContext reductionContext);
  static Expression ShallowReduceAdvancedFunction(
      Expression& e, ReductionContext reductionContext);
  static Expression ShallowReduceInverseAdvancedFunction(
      Expression& e, ReductionContext reductionContext);
  static Expression ReplaceWithAdvancedFunction(Expression& e,
                                                Expression& denominator);
#endif

  /* Turn cos(4) into cos(4rad) if the angle unit is rad and cos(π) into
   * cos(π°) if the angle unit is deg, to notify the user of the current
   * angle unit she is using if she's forgetting to switch the angle unit */
  static void DeepAddAngleUnitToAmbiguousDirectFunctions(
      UserExpression& e, Preferences::AngleUnit angleUnit);

 private:
  static bool ExpressionIsTangentOrInverseOfTangent(const UserExpression& e,
                                                    bool inverse);
};

}  // namespace Poincare

#endif
