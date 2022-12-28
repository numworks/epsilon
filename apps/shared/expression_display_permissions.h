#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <poincare/expression.h>

namespace Shared {

namespace ExpressionDisplayPermissions {

bool ShouldNeverDisplayReduction(Poincare::Expression input, Poincare::Context * context);
bool ShouldOnlyDisplayApproximation(Poincare::Expression input, Poincare::Expression exactOutput, Poincare::Context * context);

}

}

#endif
