#include <poincare/expression.h>

#include "roots.h"

namespace Poincare::Roots {

SystemExpression Linear(SystemExpression a, SystemExpression b) {
  return SystemExpression::Builder(Internal::Roots::Linear(a, b));
}

SystemExpression Quadratic(SystemExpression a, SystemExpression b,
                           SystemExpression c) {
  Internal::Tree* list = Internal::Roots::Quadratic(a, b, c);
  if (list->numberOfChildren() == 1) {
    // Flat conic
    list->removeNode();
  } else if (list->numberOfChildren() == 2) {
    /* Swap the equations since conics inequalities assume their equations
     * are in a certain order to make the distinction between inside and
     * outside. */
    list->child(0)->swapWithTree(list->child(1));
  }
  return SystemExpression::Builder(list);
}

}  // namespace Poincare::Roots
