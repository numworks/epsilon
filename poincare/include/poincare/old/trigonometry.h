#ifndef POINCARE_TRIGONOMETRY_H
#define POINCARE_TRIGONOMETRY_H

#include "evaluation.h"
#include "old_expression.h"

namespace Poincare {

class Trigonometry final {
 public:
  enum class Function {
    Cosine = 0,
    Sine = 1,
  };
  static OExpression AnglePeriodInAngleUnit(Preferences::AngleUnit angleUnit);
  static OExpression PiExpressionInAngleUnit(Preferences::AngleUnit angleUnit);
  static double PiInAngleUnit(Preferences::AngleUnit angleUnit);
  static double ConvertAngleToRadian(double angle,
                                     Preferences::AngleUnit angleUnit);
  static bool IsDirectTrigonometryFunction(const OExpression& e);
  static bool IsInverseTrigonometryFunction(const OExpression& e);
  static bool IsAdvancedTrigonometryFunction(const OExpression& e);
  static bool IsInverseAdvancedTrigonometryFunction(const OExpression& e);
  static bool AreInverseFunctions(const OExpression& directFunction,
                                  const OExpression& inverseFunction);
  /* Returns a (unreduced) division between pi in each unit, or 1 if the units
   * are the same. */
  static OExpression UnitConversionFactor(Preferences::AngleUnit fromUnit,
                                          Preferences::AngleUnit toUnit);
  static bool ExpressionIsEquivalentToTangent(const OExpression& e);
  static bool ExpressionIsEquivalentToInverseOfTangent(const OExpression& e);
  static OExpression ShallowReduceDirectFunction(
      OExpression& e, ReductionContext reductionContext);
  static OExpression ShallowReduceInverseFunction(
      OExpression& e, ReductionContext reductionContext);
  static OExpression ShallowReduceAdvancedFunction(
      OExpression& e, ReductionContext reductionContext);
  static OExpression ShallowReduceInverseAdvancedFunction(
      OExpression& e, ReductionContext reductionContext);
  static OExpression ReplaceWithAdvancedFunction(OExpression& e,
                                                 OExpression& denominator);

  template <typename T>
  static std::complex<T> ConvertToRadian(const std::complex<T> c,
                                         Preferences::AngleUnit angleUnit);
  template <typename T>
  static std::complex<T> ConvertRadianToAngleUnit(
      const std::complex<T> c, Preferences::AngleUnit angleUnit);

  /* Turn cos(4) into cos(4rad) if the angle unit is rad and cos(π) into
   * cos(π°) if the angle unit is deg, to notify the user of the current
   * angle unit she is using if she's forgetting to switch the angle unit */
  static OExpression DeepAddAngleUnitToAmbiguousDirectFunctions(
      OExpression& e, Preferences::AngleUnit angleUnit);

 private:
  static bool ExpressionIsTangentOrInverseOfTangent(const OExpression& e,
                                                    bool inverse);
};

}  // namespace Poincare

#endif
