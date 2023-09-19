#include "parametric.h"

#include <poincare_junior/src/memory/pattern_matching.h>

#include "integer.h"
#include "k_tree.h"
#include "matrix.h"
#include "simplification.h"
#include "variables.h"

namespace PoincareJ {

bool Parametric::SimplifySumOrProduct(Tree* expr) {
  bool isSum = expr->type() == BlockType::Sum;
  Tree* lowerBound = expr->childAtIndex(k_lowerBoundIndex);
  Tree* upperBound = lowerBound->nextTree();
  Tree* child = upperBound->nextTree();
  if (!Variables::HasVariable(child, k_localVariableId)) {
    // TODO : add ceil around bounds
    constexpr KTree numberOfTerms = KAdd(1_e, KA, KMult(-1_e, KB));
    Variables::LeaveScope(child);
    Tree* result = PatternMatching::CreateAndSimplify(
        isSum ? KMult(numberOfTerms, KC) : KPow(KC, numberOfTerms),
        {.KA = upperBound, .KB = lowerBound, .KC = child});
    expr->moveTreeOverTree(result);
    return true;
  }
  // TODO upperBound < lowerBound -> 0
  // TODO distribute multiplicative constant
  return false;
}

bool Parametric::ExpandSumOrProduct(Tree* expr) {
  if (!expr->type().isOfType({BlockType::Sum, BlockType::Product})) {
    return false;
  }
  // TODO is it worth to split the child in a part that depends on k ?
  return
      // split sum
      PatternMatching::MatchReplaceAndSimplify(
          expr, KSum(KA, KB, KC, KAdd(KD, KTE)),
          KAdd(KSum(KA, KB, KC, KD), KSum(KA, KB, KC, KAdd(KTE)))) ||
      // split product
      PatternMatching::MatchReplaceAndSimplify(
          expr, KProduct(KA, KB, KC, KMult(KD, KTE)),
          KMult(KProduct(KA, KB, KC, KD), KProduct(KA, KB, KC, KMult(KTE)))) ||
      // n*(n-1)/2 with any lower bound
      PatternMatching::MatchReplaceAndSimplify(
          expr, KSum(KA, KB, KC, KVar<0>),
          KAdd(KMult(KHalf, KC, KAdd(1_e, KC)),
               KMult(-1_e, KHalf, KB, KAdd(-1_e, KB))));

  // TODO well-known forms
}

// TODO try swapping sigmas

bool Parametric::ContractSumOrProduct(Tree* expr) {
  // TODO summation variables must be normalized to use pattern matching
  // different children equal bounds
  // identical children where leftUpperBound + 1 = rightLowerBound
  // product from/to factorial
  // expand and contract distribution with exp/log
  return false;
}

bool Parametric::Explicit(Tree* expr) {
  assert(expr->type() == BlockType::Sum || expr->type() == BlockType::Product);
  bool isSum = expr->type() == BlockType::Sum;
  Tree* lowerBound = expr->childAtIndex(k_lowerBoundIndex);
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
  Tree* result;
  if (isSum) {
    Dimension d = Dimension::GetDimension(child);
    result = d.isMatrix() ? Matrix::Zero(d.matrix) : (0_e)->clone();
  } else {
    result = (1_e)->clone();
  }
  for (uint8_t step = 0; step < numberOfTerms; step++) {
    Tree* n = Integer::Push(step);
    Tree* value = PatternMatching::CreateAndSimplify(
        KAdd(KA, KB), {.KA = lowerBound, .KB = n});
    n->removeTree();
    value = n;
    Tree* clone = child->clone();
    Variables::Replace(clone, k_localVariableId, value);
    value->removeTree();
    result->cloneNodeAtNode(isSum ? KAdd.node<2> : KMult.node<2>);
    // Terms are simplified one at a time to avoid overflowing the pool
    Simplification::ShallowSystematicReduce(result);
  }
  expr->moveTreeOverTree(result);
  return true;
}

}  // namespace PoincareJ
