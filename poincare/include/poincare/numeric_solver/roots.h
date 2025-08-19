#ifndef POINCARE_SOLVER_ROOTS_H
#define POINCARE_SOLVER_ROOTS_H

#include <poincare/expression.h>

namespace Poincare::Roots {

SystemExpression Linear(SystemExpression a, SystemExpression b);

SystemExpression Quadratic(SystemExpression a, SystemExpression b,
                           SystemExpression c);

}  // namespace Poincare::Roots

#endif
