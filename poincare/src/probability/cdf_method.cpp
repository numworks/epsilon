#include "cdf_method.h"

#include <poincare/src/expression/k_tree.h>

namespace Poincare::Internal {

bool CDFMethod::shallowReduce(const Tree** abscissae,
                              const Distribution* distribution,
                              const Tree** parameters, Tree* expression) const {
  const Tree* x = abscissae[0];

  // TODO_PCJ: -inf
  if (x->isInf()) {
    if (true /* x.isPositive(reductionContext.context()) == TrinaryBoolean::False */) {
      expression->cloneTreeOverTree(0_e);
    } else {
      expression->cloneTreeOverTree(1_e);
    }
    return true;
  }

  return false;
}

}  // namespace Poincare::Internal
