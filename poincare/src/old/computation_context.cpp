#include <poincare/old/computation_context.h>
#include <poincare/old/expression.h>

namespace Poincare {

void ComputationContext::updateComplexFormat(const JuniorExpression e) {
  m_complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(
      m_complexFormat, e, m_context);
}

}  // namespace Poincare
