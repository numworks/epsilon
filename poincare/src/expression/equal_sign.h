#pragma once

#include <poincare/print_float.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {
// Exact and approximated must be system expressions
bool IsCalculationOutputStrictEquality(const Tree* exact,
                                       const Tree* approximate,
                                       int significantDigits);

}  // namespace Poincare::Internal
