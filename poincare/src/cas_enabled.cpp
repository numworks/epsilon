#include <poincare/cas.h>

namespace Poincare {

bool CAS::Enabled() { return true; }

bool CAS::NeverDisplayReductionOfInput(const UserExpression& input,
                                       Context* context) {
  // FIXME Implement CAS
  return false;
}

bool CAS::ShouldOnlyDisplayApproximation(
    const UserExpression& input, const UserExpression& exactOutput,
    const UserExpression& approximateOutput, Context* context) {
  // FIXME Implement CAS
  return false;
}

}  // namespace Poincare
