#include "scientific_notation_helper.h"

#include <poincare/expression.h>

using namespace Poincare;

namespace Calculation {

namespace ScientificNotationHelper {

Layout ScientificLayout(
    const Expression a, Context* context,
    const Preferences::CalculationPreferences calculationPreferences) {
  assert(!a.hasUnit());
  assert(calculationPreferences.displayMode !=
         Preferences::PrintFloatMode::Scientific);
  Expression e;
  if (a.type() == ExpressionNode::Type::BasedInteger ||
      (a.type() == ExpressionNode::Type::Opposite &&
       a.childAtIndex(0).type() == ExpressionNode::Type::BasedInteger)) {
    // Based Integer must be approximated to be layouted in scientific mode
    ApproximationContext approximationContext(context);
    e = NewExpression::Builder<double>(
        a.approximateToScalar<double>(approximationContext));
  } else {
    e = a;
  }
  return e.createLayout(Preferences::PrintFloatMode::Scientific,
                        calculationPreferences.numberOfSignificantDigits,
                        context);
}

}  // namespace ScientificNotationHelper

}  // namespace Calculation
