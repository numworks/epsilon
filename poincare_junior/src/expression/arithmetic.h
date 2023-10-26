#ifndef POINCARE_EXPRESSION_ARITHMETIC_H
#define POINCARE_EXPRESSION_ARITHMETIC_H

#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {

class Arithmetic {
 public:
  static bool SimplifyQuotient(Tree* expr) {
    return SimplifyQuotientOrRemainder(expr);
  }
  static bool SimplifyRemainder(Tree* expr) {
    return SimplifyQuotientOrRemainder(expr);
  }

 private:
  static bool SimplifyQuotientOrRemainder(Tree* expr);
};

}  // namespace PoincareJ
#endif
