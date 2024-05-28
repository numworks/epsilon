#ifndef POINCARE_EXPRESSION_SYSTEMATIC_OPERATION_H
#define POINCARE_EXPRESSION_SYSTEMATIC_OPERATION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class SystematicOperation {
 public:
  static bool SimplifyMultiplication(Tree* u);
  EDITION_REF_WRAP(SimplifyMultiplication);

  static bool SimplifyAddition(Tree* u);
  EDITION_REF_WRAP(SimplifyAddition);
};

}  // namespace Poincare::Internal

#endif
