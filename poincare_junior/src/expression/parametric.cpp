#include "parametric.h"

#include <poincare_junior/src/memory/pattern_matching.h>

#include "integer.h"
#include "k_tree.h"
#include "simplification.h"
#include "variables.h"

namespace PoincareJ {

bool Parametric::Explicit(Tree* expr) {
  Tree* variable = expr->firstChild();
  Tree* lowerBound = variable->nextTree();
  Tree* upperBound = lowerBound->nextTree();
  Tree* child = upperBound->nextTree();
  Tree* boundsDifference = PatternMatching::CreateAndSimplify(
      KAdd(KA, KMult(-1_e, KB)), {.KA = upperBound, .KB = lowerBound});
  // TODO larger type than uint8
  if (!Integer::IsUint8(boundsDifference)) {
    boundsDifference->removeTree();
    return false;
  }
  uint8_t numberOfTerms = Integer::Uint8(boundsDifference) + 1;
  boundsDifference->removeTree();
  Tree* result = (0_e)->clone();
  for (uint8_t step = 0; step < numberOfTerms; step++) {
    Tree* n = Integer::Push(step);
    Tree* value = PatternMatching::CreateAndSimplify(
        KAdd(KA, KB), {.KA = lowerBound, .KB = n});
    n->removeTree();
    value = n;
    Tree* clone = child->clone();
    Variables::Replace(clone, variable, value);
    value->removeTree();
    Tree* add = SharedEditionPool->push<BlockType::Addition>(2);
    result->moveNodeAtNode(add);
    // Terms are simplified one at a time to avoid overflowing the pool
    Simplification::SimplifyAddition(result);
  }
  expr->moveTreeOverTree(result);
  return true;
}

}  // namespace PoincareJ
