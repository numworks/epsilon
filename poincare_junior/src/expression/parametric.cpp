#include "parametric.h"

#include <poincare_junior/src/memory/pattern_matching.h>

#include "integer.h"
#include "k_tree.h"
#include "simplification.h"
#include "variables.h"

namespace PoincareJ {

bool Parametric::Explicit(Tree* expr) {
  assert(expr->type() == BlockType::Sum || expr->type() == BlockType::Product);
  bool isSum = expr->type() == BlockType::Sum;
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
  Tree* result = (isSum ? 0_e : 1_e)->clone();
  for (uint8_t step = 0; step < numberOfTerms; step++) {
    Tree* n = Integer::Push(step);
    Tree* value = PatternMatching::CreateAndSimplify(
        KAdd(KA, KB), {.KA = lowerBound, .KB = n});
    n->removeTree();
    value = n;
    Tree* clone = child->clone();
    Variables::Replace(clone, variable, value);
    value->removeTree();
    result->cloneNodeAtNode(isSum ? KAdd.node<2> : KMult.node<2>);
    // Terms are simplified one at a time to avoid overflowing the pool
    Simplification::ShallowSystematicReduce(result);
  }
  expr->moveTreeOverTree(result);
  return true;
}

}  // namespace PoincareJ
