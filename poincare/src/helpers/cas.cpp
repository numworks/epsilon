#include <poincare/cas.h>
#include <poincare/src/expression/cas.h>

namespace Poincare {

bool CAS::Enabled() { return Internal::CAS::Enabled(); }

bool CAS::NeverDisplayReductionOfInput(const UserExpression& input,
                                       const SymbolContext& symbolContext) {
  return Internal::CAS::NeverDisplayReductionOfInput(input, symbolContext);
}

bool CAS::ShouldOnlyDisplayApproximation(
    const UserExpression& input, const UserExpression& exactOutput,
    const UserExpression& approximateOutput,
    const SymbolContext& symbolContext) {
  return Internal::CAS::ShouldOnlyDisplayApproximation(
      input, exactOutput, approximateOutput, symbolContext);
}

}  // namespace Poincare
