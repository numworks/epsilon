#include <poincare/cas.h>
#include <poincare/src/expression/cas.h>

namespace Poincare {

bool CAS::Enabled() { return Internal::CAS::Enabled(); }

bool CAS::NeverDisplayReductionOfInput(const UserExpression& input,
                                       const Context& ctx) {
  return Internal::CAS::NeverDisplayReductionOfInput(input, ctx);
}

bool CAS::ShouldOnlyDisplayApproximation(
    const UserExpression& input, const UserExpression& exactOutput,
    const UserExpression& approximateOutput, const Context& ctx) {
  return Internal::CAS::ShouldOnlyDisplayApproximation(input, exactOutput,
                                                       approximateOutput, ctx);
}

}  // namespace Poincare
