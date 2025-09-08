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
  PoincareHelpers::CloneAndSimplify(
      &norm, symbolContext, calculationPreferences.complexFormat,
      calculationPreferences.angleUnit, true, Poincare::ReductionTarget::User,
      VectorListController::k_symbolicComputation, &reductionFailure);
  return reductionFailure || norm.isUninitialized() || norm.isUndefined()
             ? UserExpression()
             : norm;
}

}  // namespace VectorHelper

}  // namespace Calculation
