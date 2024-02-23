#ifndef POINCARE_JUNIOR_EXPRESSION_CONVERSION_H
#define POINCARE_JUNIOR_EXPRESSION_CONVERSION_H

#include <poincare/expression.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

/* Temporary bi-directional conversions between old and new poincare to smooth
 * transition. */

Poincare::OExpression ToPoincareExpression(const Tree *exp);
void PushPoincareExpression(Poincare::OExpression exp);
Tree *FromPoincareExpression(Poincare::OExpression exp);

}  // namespace PoincareJ

#endif
