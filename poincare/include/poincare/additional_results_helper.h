#ifndef POINCARE_ADDITIONAL_RESULTS_HELPER_H
#define POINCARE_ADDITIONAL_RESULTS_HELPER_H

#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/src/expression/projection.h>

namespace Poincare {

class AdditionalResultsHelper final {
 public:
  /* Trigonometry additional results */
  /* Shared::ShouldOnlyDisplayApproximation is used in TrigonometryAngleHelper
   * and passed here as parameter. */
  typedef bool (*ShouldOnlyDisplayApproximation)(
      const UserExpression& input, const UserExpression& exactOutput,
      const UserExpression& approximateOutput, Context* context);

  struct TrigonometryResults {
    UserExpression exactAngle;
    float approximatedAngle;
    bool angleIsExact;
  };
  static TrigonometryResults TrigonometryAngleHelper(
      const UserExpression input, const UserExpression exactOutput,
      const UserExpression approximateOutput, bool directTrigonometry,
      Preferences::CalculationPreferences calculationPreferences,
      const Internal::ProjectionContext* ctx,
      ShouldOnlyDisplayApproximation shouldOnlyDisplayApproximation);

  static UserExpression ExtractExactAngleFromDirectTrigo(
      const UserExpression input, const UserExpression exactOutput,
      Context* context,
      const Preferences::CalculationPreferences calculationPreferences);

  static bool expressionIsInterestingFunction(const UserExpression e);

  static bool HasInverseTrigo(const UserExpression input,
                              const UserExpression exactOutput,
                              const UserExpression approximateOutput);

  /* Function additional results */
  static bool HasSingleNumericalValue(const UserExpression input);
  static UserExpression CloneReplacingNumericalValuesWithSymbol(
      const UserExpression input, const char* symbol, float* value);

  /* Integer additional results */
  /* Return the integer UserExpression from exactOutput representing an
   * integer (Float, Double or Integer) */
  static UserExpression EquivalentInteger(const UserExpression exactOutput);
  /* Return true if exactOutput represents a positive integer (Float, Double or
   * Integer) */
  static bool HasPositiveInteger(const UserExpression exactOutput);
  constexpr static uint64_t k_maxPositiveInteger = 10000000000000000;

  /* Rational additional results */
  static bool HasRational(const UserExpression exactOutput);
  static SystemExpression CreateRational(const UserExpression e, bool negative);
  static UserExpression CreateEuclideanDivision(SystemExpression rational);
  static UserExpression CreateMixedFraction(SystemExpression rational,
                                            bool mixedFractionsEnabled);

  /* Scientific notation additional results */
  static Layout ScientificLayout(
      const UserExpression approximateOutput, Context* context,
      const Preferences::CalculationPreferences calculationPreferences);

  /* Matrix additional results */
  static void ComputeMatrixProperties(
      const UserExpression& exactOutput,
      const UserExpression& approximateOutput, Internal::ProjectionContext ctx,
      Preferences::PrintFloatMode displayMode,
      uint8_t numberOfSignificantDigits, Layout& determinant, Layout& inverse,
      Layout& rowEchelonForm, Layout& reducedRowEchelonForm, Layout& trace);
};

}  // namespace Poincare

#endif
