#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <poincare/cas.h>

namespace Shared {

namespace ExpressionDisplayPermissions {

// This needs to be exposed for the solver
inline bool NeverDisplayReductionOfInput(Poincare::UserExpression input,
                                         Poincare::Context* context) {
  return Poincare::CAS::NeverDisplayReductionOfInput(input, context);
}

inline bool ShouldOnlyDisplayApproximation(
    Poincare::UserExpression input, Poincare::UserExpression exactOutput,
    Poincare::UserExpression approximateOutput, Poincare::Context* context) {
  return Poincare::CAS::ShouldOnlyDisplayApproximation(
      input, exactOutput, approximateOutput, context);
}

}  // namespace ExpressionDisplayPermissions

}  // namespace Shared

#endif
