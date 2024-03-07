#include "scientific_notation_helper.h"

#include <poincare/float.h>

using namespace Poincare;

namespace Calculation {

namespace ScientificNotationHelper {

Layout ScientificLayout(const Expression a, Context* context) {
  assert(!a.hasUnit());
  assert(Preferences::sharedPreferences->displayMode() !=
         Preferences::PrintFloatMode::Scientific);
  Expression e;
  if (a.type() == ExpressionNode::Type::BasedInteger) {
    // Based Integer must be approximated to be layouted in scientific mode
    ApproximationContext approximationContext(context);
    e = Float<double>::Builder(
        a.approximateToScalar<double>(approximationContext));
  } else {
    e = a;
  }
  return e.createLayout(
      Preferences::PrintFloatMode::Scientific,
      Preferences::sharedPreferences->numberOfSignificantDigits(), context);
}

}  // namespace ScientificNotationHelper

}  // namespace Calculation
