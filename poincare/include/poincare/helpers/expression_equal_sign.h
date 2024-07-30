#ifndef POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H
#define POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H

namespace Poincare {
namespace Internal {
class Tree;
}

// Exact and approximated must be system expressions
bool ExactAndApproximateExpressionsAreEqual(const Internal::Tree* exact,
                                            const Internal::Tree* approximated);

}  // namespace Poincare

#endif
