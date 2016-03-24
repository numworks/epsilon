#include "simplify_product_zero.h"
#include <poincare/product.h>
#include <poincare/integer.h>

Expression * SimplifyProductZero(Expression * e) {
  if (e->type() != Expression::Type::Product) {
    return nullptr;
  }
  Product * p = (Product *)e;
  for (int i=0; i<p->numberOfOperands(); i++) {
    Expression * factor = p->operand(i);
    if (factor->type() == Expression::Type::Integer) {
      Integer * integer = (Integer *)factor;
      if (*integer == Integer((native_int_t)0)) {
        return new Integer("0");
      }
    }
  }
  return nullptr;
}
