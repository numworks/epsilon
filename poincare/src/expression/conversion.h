#ifndef POINCARE_EXPRESSION_CONVERSION_H
#define POINCARE_EXPRESSION_CONVERSION_H

#include <poincare/expression.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

/* Temporary bi-directional conversions between old and new poincare to smooth
 * transition. */

Poincare::OExpression ToPoincareExpression(const Tree* e);
void PushPoincareExpression(Poincare::OExpression exp);
Tree* FromPoincareExpression(Poincare::OExpression exp);

}  // namespace Poincare::Internal

#endif
