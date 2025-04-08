#include "integration.h"

#include "advanced_reduction.h"
#include "parametric.h"

namespace Poincare::Internal {

bool Integration::Reduce(Tree* e) {
  assert(e->isIntegral());
  const Tree* integrand = e->child(Parametric::k_integrandIndex);

  if (integrand->isAdd()) {
    Tree* splitIntegral =
        SharedTreeStack->pushAdd(integrand->numberOfChildren());
    for (const Tree* child : integrand->children()) {
      Tree* newChild = e->cloneTree();
      newChild->child(Parametric::k_integrandIndex)->cloneTreeOverTree(child);
    }
    e->moveTreeOverTree(splitIntegral);
    return true;
  }
  return false;
}

}  // namespace Poincare::Internal
