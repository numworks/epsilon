#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <poincare/expression.h>

namespace Shared {

namespace ExpressionDisplayPermissions {

// This needs to be exposed for the solver
bool NeverDisplayReductionOfInput(Poincare::Expression input,
                                  Poincare::Context* context);

bool ShouldOnlyDisplayApproximation(Poincare::Expression input,
                                    Poincare::Expression exactOutput,
                                    Poincare::Expression approximateOutput,
                                    Poincare::Context* context);

}  // namespace ExpressionDisplayPermissions

}  // namespace Shared

#endif
