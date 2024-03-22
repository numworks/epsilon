#ifndef POINCARE_JUNIOR_EXPRESSION_CONVERSION_H
#define POINCARE_JUNIOR_EXPRESSION_CONVERSION_H

#include <poincare/expression.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

/* Temporary bi-directional conversions between old and new poincare to smooth
 * transition. */

Poincare::Expression ToPoincareExpression(const Tree *exp);
void PushPoincareExpression(Poincare::Expression exp);
Tree *FromPoincareExpression(Poincare::Expression exp);

}  // namespace PoincareJ

#endif
