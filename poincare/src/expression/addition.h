#ifndef POINCARE_EXPRESSION_ADDITION_H
#define POINCARE_EXPRESSION_ADDITION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Addition {
 public:
  static bool SimplifyAddition(Tree* u);
  EDITION_REF_WRAP(SimplifyAddition);
};

}  // namespace Poincare::Internal

#endif
