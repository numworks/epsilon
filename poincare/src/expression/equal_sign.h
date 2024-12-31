#ifndef POINCARE_SRC_EXPRESSION_EQUAL_SIGN_H
#define POINCARE_SRC_EXPRESSION_EQUAL_SIGN_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {
// Exact and approximated must be system expressions
bool ExactAndApproximateExpressionsAreStrictlyEqual(const Tree* exact,
                                                    const Tree* approximate);

}  // namespace Poincare::Internal

#endif
