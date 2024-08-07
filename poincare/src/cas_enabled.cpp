#include <poincare/cas.h>

namespace Poincare {

bool CAS::Enabled() { return true; }

bool CAS::NeverDisplayReductionOfInput(UserExpression input, Context* context) {
  // FIXME Implement CAS
  return false;
}

bool CAS::ShouldOnlyDisplayApproximation(UserExpression input,
                                         UserExpression exactOutput,
                                         UserExpression approximateOutput,
                                         Context* context) {
  // FIXME Implement CAS
  return false;
}

}  // namespace Poincare
