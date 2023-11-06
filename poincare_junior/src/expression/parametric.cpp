#include "parametric.h"

#include <poincare_junior/src/memory/pattern_matching.h>

#include "integer.h"
#include "k_tree.h"
#include "matrix.h"
#include "simplification.h"
#include "variables.h"

namespace PoincareJ {

bool Parametric::SimplifySumOrProduct(Tree* expr) {
  bool isSum = expr->isSum();
  Tree* lowerBound = expr->child(k_lowerBoundIndex);
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

bool Parametric::ExpandSum(Tree* expr) {
  if (!expr->isSum()) {
    return false;
  }
  return
      // sum(f+g,k,a,b) = sum(f,k,a,b) + sum(g,k,a,b)
      Simplification::DistributeOverNAry(
          expr, BlockType::Sum, BlockType::Addition, BlockType::Addition,
          [](Tree* expr) -> bool {
            return Simplification::ShallowSystematicReduce(expr) +
                   ExpandSum(expr);
          },
          k_integrandIndex) ||
      PatternMatching::MatchReplaceAndSimplify(
          expr, KSum(KA, KB, KC, KAdd(KD, KTE)),
          KAdd(KSum(KA, KB, KC, KD), KSum(KA, KB, KC, KAdd(KTE)))) ||
#if 0 /* This first rule is powerful and simplifies the next two rules but it \
       * may introduced undefs with sum(1/k,k,1,2) for instance. */
      // sum(e,k,a,b) = sum(e,k,0,b) - sum(e,k,0,a-1)
      // TODO what if a < 0 ?
      (!expr->child(k_lowerBoundIndex)->isZero() &&
       PatternMatching::MatchReplaceAndSimplify(
           expr, KSum(KA, KB, KC, KD),
           KAdd(KSum(KA, 0_e, KC, KD),
                KMult(-1_e, KSum(KA, 0_e, KAdd(KB, -1_e), KD))))) ||
      // sum(k,k,0,n) = n(n+1)/2
      PatternMatching::MatchReplaceAndSimplify(
          expr, KSum(KA, 0_e, KC, KVar<0>), KMult(KHalf, KC, KAdd(1_e, KC))) ||
      // sum(k^2,k,0,n) = n(n+1)(2n+1)/6
      PatternMatching::MatchReplaceAndSimplify(
          expr, KSum(KA, 0_e, KC, KPow(KVar<0>, 2_e)),
          KMult(KC, KAdd(KC, 1_e), KAdd(KMult(2_e, KC), 1_e),
                KPow(6_e, -1_e))) ||
#else
      // sum(k,k,m,n) = n(n+1)/2 - (m-1)m/2
      PatternMatching::MatchReplaceAndSimplify(
          expr, KSum(KA, KB, KC, KVar<0>),
          KMult(KHalf, KAdd(KMult(KC, KAdd(1_e, KC)),
                            KMult(-1_e, KB, KAdd(-1_e, KB))))) ||
      // sum(k^2,k,m,n) = n(n+1)(2n+1)/6 - (m-1)(m)(2m-1)/6
      PatternMatching::MatchReplaceAndSimplify(
          expr, KSum(KA, KB, KC, KPow(KVar<0>, 2_e)),
          KMult(KPow(6_e, -1_e),
                KAdd(KMult(KC, KAdd(KC, 1_e), KAdd(KMult(2_e, KC), 1_e)),
                     KMult(-1_e, KAdd(-1_e, KB), KB,
                           KAdd(KMult(2_e, KB), -1_e))))) ||
#endif
      // sum(x_k, k, 0, n) = x_0 + ... + x_n
      Explicit(expr);
  // TODO Split the child in a part that depends on k ?
}

bool Parametric::ExpandProduct(Tree* expr) {
  if (!expr->isProduct()) {
    return false;
  }
  return
      // split product
      PatternMatching::MatchReplaceAndSimplify(
          expr, KProduct(KA, KB, KC, KMult(KD, KTE)),
          KMult(KProduct(KA, KB, KC, KD), KProduct(KA, KB, KC, KMult(KTE)))) ||
      Explicit(expr);
  ;
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
  assert(expr->isSum() || expr->isProduct());
  bool isSum = expr->isSum();
  const Tree* lowerBound = expr->child(k_lowerBoundIndex);
  const Tree* upperBound = lowerBound->nextTree();
  const Tree* child = upperBound->nextTree();
  Tree* boundsDifference = PatternMatching::CreateAndSimplify(
      KAdd(KA, KMult(-1_e, KB)), {.KA = upperBound, .KB = lowerBound});
  // TODO larger type than uint8
  if (!Integer::Is<uint8_t>(boundsDifference)) {
    boundsDifference->removeTree();
    return false;
  }
  uint8_t numberOfTerms = Integer::Handler(boundsDifference).to<uint8_t>() + 1;
  boundsDifference->removeTree();
  Tree* result;
  if (isSum) {
    Dimension d = Dimension::GetDimension(child);
    result = d.isMatrix() ? Matrix::Zero(d.matrix) : (0_e)->clone();
  } else {
    result = (1_e)->clone();
  }
  for (uint8_t step = 0; step < numberOfTerms; step++) {
    // Create k value at this step
    Tree* value = SharedEditionPool->push<BlockType::Addition>(2);
    lowerBound->clone();
    Integer::Push(step);
    Simplification::ShallowSystematicReduce(value);
    // Clone the child and replace k with its value
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
