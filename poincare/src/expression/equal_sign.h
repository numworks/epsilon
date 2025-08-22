#pragma once

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {
// Exact and approximated must be system expressions
bool ExactAndApproximateExpressionsAreStrictlyEqual(const Tree* exact,
                                                    const Tree* approximate);

}  // namespace Poincare::Internal
