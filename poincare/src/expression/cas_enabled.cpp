#include "cas.h"

namespace Poincare::Internal {

bool CAS::Enabled() { return true; }

bool CAS::NeverDisplayReductionOfInput(const Internal::Tree* input,
                                       const SymbolContext& symbolContext) {
  // FIXME Implement CAS
  return false;
}

bool CAS::ShouldOnlyDisplayApproximation(
    const Internal::Tree* input, const Internal::Tree* exactOutput,
    const Internal::Tree* approximateOutput,
    const SymbolContext& symbolContext) {
  // FIXME Implement CAS
  return false;
}

}  // namespace Poincare::Internal
