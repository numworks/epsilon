#ifndef POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H
#define POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H

#include <poincare/expression.h>

namespace Poincare {
namespace Internal {
class Tree;

// Exact and approximated must be system expressions
bool ExactAndApproximateExpressionsAreStriclyEqual(
    const Internal::Tree* exact, const Internal::Tree* approximated);
}  // namespace Internal

// Exact and approximated must be system expressions
inline bool ExactAndApproximateExpressionsAreStriclyEqual(
    SystemExpression exact, SystemExpression approximated) {
  return Internal::ExactAndApproximateExpressionsAreStriclyEqual(exact,
                                                                 approximated);
}

}  // namespace Poincare

#endif
