#include "cdf_method.h"

#include <poincare_junior/src/expression/k_tree.h>

namespace PoincareJ {

bool CDFMethod::shallowReduce(const Tree** abscissae,
                              const Distribution* distribution,
                              const Tree** parameters, Tree* expression) const {
  const Tree* x = abscissae[0];

  // TODO PCJ: -inf
  if (x->isInfinity()) {
    if (true /* x.isPositive(reductionContext.context()) == TrinaryBoolean::False */) {
      expression->cloneTreeOverTree(0_e);
    } else {
      expression->cloneTreeOverTree(1_e);
    }
    return true;
  }

  return false;
}

}  // namespace PoincareJ
