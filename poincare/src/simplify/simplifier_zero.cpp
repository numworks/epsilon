#include "simplifier_zero.h"
#include <poincare/product.h>
#include <poincare/integer.h>

Expression * SimplifierZero(Expression * e) {
  if (e->type() != Product::Type) {
    return nullptr;
  }
  Product * p = (Product *)e;
  for (int i=0; i<2; i++) {
    Expression * factor = p->m_children[i];
    if (factor->type() == Integer::Type) {
      Integer * integer = (Integer *)factor;
      if (*integer == Integer((native_int_t)0)) {
        return new Integer("0");
      }
    }
  }
  return nullptr;
}
