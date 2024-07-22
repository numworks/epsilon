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
  static Expression AnglePeriodInAngleUnit(Preferences::AngleUnit angleUnit);
  static Expression PiExpressionInAngleUnit(Preferences::AngleUnit angleUnit);
  static double PiInAngleUnit(Preferences::AngleUnit angleUnit);
  static double ConvertAngleToRadian(double angle,
                                     Preferences::AngleUnit angleUnit);
  static bool IsDirectTrigonometryFunction(const Expression& e);
  static bool IsInverseTrigonometryFunction(const Expression& e);
  static bool IsAdvancedTrigonometryFunction(const Expression& e);
  static bool IsInverseAdvancedTrigonometryFunction(const Expression& e);
  static bool AreInverseFunctions(const Expression& directFunction,
                                  const Expression& inverseFunction);
  /* Returns a (unreduced) division between pi in each unit, or 1 if the units
   * are the same. */
  static Expression UnitConversionFactor(Preferences::AngleUnit fromUnit,
                                         Preferences::AngleUnit toUnit);
  static bool ExpressionIsEquivalentToTangent(const Expression& e);
  static bool ExpressionIsEquivalentToInverseOfTangent(const Expression& e);
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

  template <typename T>
  static std::complex<T> ConvertToRadian(const std::complex<T> c,
                                         Preferences::AngleUnit angleUnit);
  template <typename T>
  static std::complex<T> ConvertRadianToAngleUnit(
      const std::complex<T> c, Preferences::AngleUnit angleUnit);

  /* Turn cos(4) into cos(4rad) if the angle unit is rad and cos(π) into
   * cos(π°) if the angle unit is deg, to notify the user of the current
   * angle unit she is using if she's forgetting to switch the angle unit */
  static Expression DeepAddAngleUnitToAmbiguousDirectFunctions(
      Expression& e, Preferences::AngleUnit angleUnit);

 private:
  static bool ExpressionIsTangentOrInverseOfTangent(const Expression& e,
                                                    bool inverse);
};

}  // namespace Poincare

#endif
