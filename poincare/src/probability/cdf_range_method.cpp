#include "cdf_range_method.h"

#include <poincare/src/expression/infinity.h>
#include <poincare/src/expression/k_tree.h>

namespace Poincare::Internal {

bool CDFRangeMethod::shallowReduce(const Tree** abscissae,
                                   const Distribution* distribution,
                                   const Tree** parameters,
                                   Tree* expression) const {
  const Tree* x = abscissae[0];
  const Tree* y = abscissae[1];

  if (Infinity::TreeIsMinusInfinity(x)) {
    if (y->isInf()) {
      expression->cloneTreeOverTree(1_e);
      return true;
    }
    if (Infinity::TreeIsMinusInfinity(y)) {
      expression->cloneTreeOverTree(0_e);
      return true;
    }
    /* TODO: return CDF of the same distributions with the same parameters
     * tcdfrange(-inf, 4, 5) => tcdf(4, 5) */
  }

  return false;
}

}  // namespace Poincare::Internal
