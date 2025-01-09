#include "cas.h"

namespace Poincare::Internal {

bool CAS::Enabled() { return true; }

bool CAS::NeverDisplayReductionOfInput(const Internal::Tree* input,
                                       Context* context) {
  // FIXME Implement CAS
  return false;
}

bool CAS::ShouldOnlyDisplayApproximation(
    const Internal::Tree* input, const Internal::Tree* exactOutput,
    const Internal::Tree* approximateOutput, Context* context) {
  // FIXME Implement CAS
  return false;
}

}  // namespace Poincare::Internal
