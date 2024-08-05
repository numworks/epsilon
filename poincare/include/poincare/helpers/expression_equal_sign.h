#ifndef POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H
#define POINCARE_HELPERS_EXPRESSION_EQUAL_SIGN_H

#include <poincare/expression.h>

namespace Poincare {
bool ExactAndApproximateExpressionsAreStriclyEqual(UserExpression exact,
                                                   UserExpression approximate);

}  // namespace Poincare

#endif
