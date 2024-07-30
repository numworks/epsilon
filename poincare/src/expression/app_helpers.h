#ifndef POINCARE_EXPRESSION_APP_HELPERS_H
#define POINCARE_EXPRESSION_APP_HELPERS_H

#include <poincare/src/memory/tree.h>

namespace Poincare {
namespace Internal {
class Tree;
}

namespace AppHelpers {

// Exact and approximated must be system expressions
bool ExactAndApproximateExpressionsAreEqual(const Internal::Tree* exact,
                                            const Internal::Tree* approximated);

}  // namespace AppHelpers
}  // namespace Poincare

#endif
