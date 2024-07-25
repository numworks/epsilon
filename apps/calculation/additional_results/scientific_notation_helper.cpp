#include "scientific_notation_helper.h"

#include <poincare/expression.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/memory/tree.h>

using namespace Poincare;
using namespace Poincare::Internal;

namespace Calculation {

namespace ScientificNotationHelper {

Poincare::Layout ScientificLayout(
    const UserExpression approximateOutput, Context* context,
    const Preferences::CalculationPreferences calculationPreferences) {
  assert(calculationPreferences.displayMode !=
         Preferences::PrintFloatMode::Scientific);

  ProjectionContext ctx = {
      .m_strategy = Strategy::ApproximateToFloat,
      .m_symbolic =
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
      .m_context = context};

  Tree* e = approximateOutput.tree()->cloneTree();
  Simplification::ProjectAndReduce(e, &ctx, false);
  assert(!ctx.m_dimension.isUnit());
  Simplification::BeautifyReduced(e, &ctx);
  return JuniorLayout::Builder(Layouter::LayoutExpression(
      e, false, calculationPreferences.numberOfSignificantDigits,
      Preferences::PrintFloatMode::Scientific));
}

}  // namespace ScientificNotationHelper

}  // namespace Calculation
