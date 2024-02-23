#include <poincare/computation_context.h>
#include <poincare/expression.h>

namespace Poincare {

void ComputationContext::updateComplexFormat(const OExpression e) {
  m_complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(
      m_complexFormat, e, m_context);
}

}  // namespace Poincare
