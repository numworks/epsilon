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
  static SystemExpression CreateEuclideanDivision(SystemExpression e);
  static SystemExpression CreateRational(const UserExpression e, bool negative);
  static SystemExpression CreateMixedFraction(SystemExpression rational,
                                              bool mixedFractionsEnabled);

  /* Scientific notation additional results */
  static Layout ScientificLayout(
      const UserExpression approximateOutput, Context* context,
      const Preferences::CalculationPreferences calculationPreferences);

  /* Matrix additional results */
  static void ComputeMatrixProperties(const UserExpression& exactOutput,
                                      const UserExpression& approximateOutput,
                                      Internal::ProjectionContext ctx,
                                      Layout& determinant, Layout& inverse,
                                      Layout& rowEchelonForm,
                                      Layout& reducedRowEchelonForm,
                                      Layout& trace);
};

}  // namespace Poincare

#endif
