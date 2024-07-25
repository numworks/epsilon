#ifndef POINCARE_ADDITIONAL_RESULT_HELPER_H
#define POINCARE_ADDITIONAL_RESULT_HELPER_H

#include <poincare/expression.h>

namespace Poincare {

class AdditionalResultHelper final {
 public:
  static void TrigonometryAngleHelper(
      const UserExpression input, const UserExpression exactOutput,
      const UserExpression approximateOutput, bool directTrigonometry,
      Poincare::Preferences::CalculationPreferences calculationPreferences,
      const Internal::ProjectionContext* ctx, UserExpression& exactAngle,
      float* approximatedAngle, bool* angleIsExact);
  static UserExpression ExtractExactAngleFromDirectTrigo(
      const UserExpression input, const UserExpression exactOutput,
      Context* context,
      const Preferences::CalculationPreferences calculationPreferences);
};

}  // namespace Poincare

#endif
