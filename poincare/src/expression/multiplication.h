#ifndef POINCARE_EXPRESSION_MULTIPLICATION_H
#define POINCARE_EXPRESSION_MULTIPLICATION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Multiplication {
 public:
  static bool SimplifyMultiplication(Tree* u);
  EDITION_REF_WRAP(SimplifyMultiplication);
};

}  // namespace Poincare::Internal

#endif
