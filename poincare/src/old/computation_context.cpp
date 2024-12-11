#include <poincare/expression.h>
#include <poincare/old/computation_context.h>

namespace Poincare {

void ComputationContext::updateComplexFormat(const Expression e) {
  m_complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(
      m_complexFormat, e, m_context);
}

}  // namespace Poincare
