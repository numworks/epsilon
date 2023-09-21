#include <poincare/computation_context.h>
#include <poincare/expression.h>

namespace Poincare {

void ComputationContext::updateComplexFormat(bool updateComplexFormat,
                                             const Expression e) {
  if (updateComplexFormat) {
    m_complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(
        m_complexFormat, e, m_context);
  }
}

}  // namespace Poincare