#ifndef POINCARE_EXPRESSION_INTEGRATION_H
#define POINCARE_EXPRESSION_INTEGRATION_H

#include <poincare/src/memory/tree_ref.h>

namespace Poincare::Internal {

class Integration {
 public:
  static bool Reduce(Tree* e);
};

}  // namespace Poincare::Internal

#endif
