#include "cdf_method.h"

#include <poincare/src/expression/infinity.h>
#include <poincare/src/expression/k_tree.h>

namespace Poincare::Internal {

bool CDFMethod::shallowReduce(const Tree** abscissae,
                              const Distribution* distribution,
                              const Tree** parameters, Tree* expression) const {
  const Tree* x = abscissae[0];

  if (x->isInf()) {
    expression->cloneTreeOverTree(1_e);
    return true;
  }
  if (Infinity::TreeIsMinusInfinity(x)) {
    expression->cloneTreeOverTree(0_e);
    return true;
  }

  return false;
}

}  // namespace Poincare::Internal
