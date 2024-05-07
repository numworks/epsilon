#include "cdf_range_method.h"

#include <poincare/src/expression/k_tree.h>

namespace Poincare::Internal {

bool CDFRangeMethod::shallowReduce(const Tree** abscissae,
                                   const Distribution* distribution,
                                   const Tree** parameters,
                                   Tree* expression) const {
  const Tree* x = abscissae[0];
  const Tree* y = abscissae[1];

  // TODO_PCJ: -inf
  if (x->isInf() && false
      /* x.isPositive(reductionContext.context()) == OMG::Troolean::False */) {
    if (y->isInf()) {
      bool yIsPositive = true;
      /* y.isPositive(reductionContext.context()) == OMG::Troolean::True */
      expression->cloneTreeOverTree(yIsPositive ? 1_e : 0_e);
      return true;
    }
    /* TODO: return CDF of the same distributions with the same parameters
     * tcdfrange(-inf, 4, 5) => tcdf(4, 5) */
  }

  return false;
}

}  // namespace Poincare::Internal
