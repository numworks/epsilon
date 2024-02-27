#include "vector_helper.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/matrix.h>
#include <poincare/vector_norm.h>

#include "vector_list_controller.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

namespace VectorHelper {

Expression BuildVectorNorm(
    const Expression exactOutput, Context *context,
    const Preferences::CalculationPreferences calculationPreferences) {
  assert(!exactOutput.isUninitialized());
  assert(!exactOutput.hasUnit(true));
  if (exactOutput.type() != ExpressionNode::Type::Matrix ||
      !static_cast<const Matrix &>(exactOutput).isVector()) {
    return Expression();
  }
  Expression norm = VectorNorm::Builder(exactOutput);
  Preferences::ComplexFormat complexFormat =
      calculationPreferences.complexFormat;
  Preferences::AngleUnit angleUnit = calculationPreferences.angleUnit;
  PoincareHelpers::CloneAndSimplify(
      &norm, context,
      {.complexFormat = complexFormat,
       .angleUnit = angleUnit,
       .target = VectorListController::k_target,
       .symbolicComputation = VectorListController::k_symbolicComputation});
  return norm.isUninitialized() || norm.isUndefined() ? Expression() : norm;
}

}  // namespace VectorHelper

}  // namespace Calculation
