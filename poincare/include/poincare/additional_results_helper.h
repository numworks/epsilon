#ifndef POINCARE_ADDITIONAL_RESULTS_HELPER_H
#define POINCARE_ADDITIONAL_RESULTS_HELPER_H

#include <poincare/expression.h>

namespace Poincare {

class AdditionalResultsHelper final {
 public:
  /* Trigonometry additional results */
  /* Shared::ShouldOnlyDisplayApproximation is used in TrigonometryAngleHelper
   * and passed here as parameter. */
  typedef bool (*ShouldOnlyDisplayApproximation)(
      const UserExpression& input, const UserExpression& exactOutput,
      const UserExpression& approximateOutput, Context* context);
  static void TrigonometryAngleHelper(
      const UserExpression input, const UserExpression exactOutput,
      const UserExpression approximateOutput, bool directTrigonometry,
      Preferences::CalculationPreferences calculationPreferences,
      const Internal::ProjectionContext* ctx,
      ShouldOnlyDisplayApproximation shouldOnlyDisplayApproximation,
      UserExpression& exactAngle, float* approximatedAngle, bool* angleIsExact);

  static UserExpression ExtractExactAngleFromDirectTrigo(
      const UserExpression input, const UserExpression exactOutput,
      Context* context,
      const Preferences::CalculationPreferences calculationPreferences);

  /* Function additional results */
  static bool HasSingleNumericalValue(const UserExpression input);
  static UserExpression CloneReplacingNumericalValuesWithSymbol(
      const UserExpression input, const char* symbol, float* value);

  /* Integer additional results */
  static bool HasInteger(const UserExpression exactOutput);

  /* Rational additional results */
  static bool HasRational(const UserExpression exactOutput);
};

}  // namespace Poincare

#endif
