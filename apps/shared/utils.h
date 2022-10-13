#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H

#include <poincare/expression.h>

namespace Shared {

namespace Utils {

bool ShouldOnlyDisplayApproximation(Poincare::Expression input, Poincare::Expression exactOutput, Poincare::Context * context);
bool ShouldOnlyDisplayExactOutput(Poincare::Expression input);

}

}

#endif
