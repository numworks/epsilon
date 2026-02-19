#include "vector_helper.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/k_tree.h>

#include "vector_list_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

namespace VectorHelper {

UserExpression BuildVectorNorm(
    const UserExpression exactOutput, const SymbolContext& symbolContext,
    const Preferences::CalculationPreferences calculationPreferences) {
  assert(!exactOutput.isUninitialized());
  assert(!exactOutput.hasUnit(true));
  if (!exactOutput.dimension().isVector()) {
    return UserExpression();
  }

  UserExpression norm = UserExpression::Create(KNorm(KA), {.KA = exactOutput});
  bool reductionFailure = false;

  assert(calculationPreferences.complexFormat ==
         norm.preferredComplexFormat(
             calculationPreferences.complexFormat, symbolContext,
             VectorListController::k_symbolicComputation));
  norm = norm.cloneAndSimplify(
      SimplificationParameters{
          .complexFormat = calculationPreferences.complexFormat,
          .angleUnit = calculationPreferences.angleUnit,
          .reduction = {.target = ReductionTarget::User},
          .symbolic = VectorListController::k_symbolicComputation,
          .context = symbolContext,
      },
      &reductionFailure);

  return reductionFailure || norm.isUninitialized() || norm.isUndefined()
             ? UserExpression()
             : norm;
}

}  // namespace VectorHelper

}  // namespace Calculation
