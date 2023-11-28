#include "scientific_notation_helper.h"

#include <apps/shared/poincare_helpers.h>
#include <poincare/float.h>

using namespace Poincare;

namespace Calculation {

namespace ScientificNotationHelper {

bool HasAdditionalOutputs(const Expression a, Context* context) {
  Layout historyResult = Shared::PoincareHelpers::CreateLayout(a, context);
  return !historyResult.isIdenticalTo(ScientificLayout(a, context));
}

Layout ScientificLayout(const Expression a, Context* context) {
  assert(!a.hasUnit());
  if (a.type() != ExpressionNode::Type::BasedInteger) {
    return Shared::PoincareHelpers::CreateLayout(
        a, context, Preferences::PrintFloatMode::Scientific);
  }
  // Based Integer must be approximated to be layouted in scientific mode
  ApproximationContext approximationContext(context);
  Expression floatRepr = Float<double>::Builder(
      a.approximateToScalar<double>(approximationContext));
  return Shared::PoincareHelpers::CreateLayout(
      floatRepr, context, Preferences::PrintFloatMode::Scientific);
}

}  // namespace ScientificNotationHelper

}  // namespace Calculation
