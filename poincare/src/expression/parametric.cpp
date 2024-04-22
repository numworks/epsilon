#include "parametric.h"

#include <poincare/src/memory/pattern_matching.h>

#include "comparison.h"
#include "integer.h"
#include "k_tree.h"
#include "matrix.h"
#include "sign.h"
#include "simplification.h"
#include "variables.h"

namespace Poincare::Internal {

uint8_t Parametric::FunctionIndex(const Tree* t) {
  return FunctionIndex(t->type());
}

uint8_t Parametric::FunctionIndex(TypeBlock type) {
  switch (type) {
    case Type::Diff:
    case Type::DiffLayout:
    case Type::ListSequence:
    case Type::ListSequenceLayout:
      return 2;
    case Type::NthDiff:
    case Type::NthDiffLayout:
    case Type::Integral:
    case Type::IntegralLayout:
    case Type::Sum:
    case Type::SumLayout:
    case Type::Product:
    case Type::ProductLayout:
      return k_integrandIndex;
    default:
      assert(false);
  }
}

ComplexSign Parametric::VariableSign(const Tree* t) {
  switch (t->type()) {
    case Type::Diff:
    case Type::NthDiff:
    case Type::Integral:
      return k_continuousVariableSign;
    case Type::ListSequence:
    case Type::Sum:
    case Type::Product:
      return k_discreteVariableSign;
    default:
      assert(false);
  }
}

bool Parametric::SimplifySumOrProduct(Tree* expr) {
  /* TODO:
   * - Distribute multiplicative constant : sum(a*f(k),k,m,n) ->
   *                                        a*(n-m)*sum(f(k),k,m,n)
   */
  bool isSum = expr->isSum();
  Tree* lowerBound = expr->child(k_lowerBoundIndex);
  Tree* upperBound = lowerBound->nextTree();
  ComplexSign sign = ComplexSign::SignOfDifference(lowerBound, upperBound);
  if (!sign.isReal()) {
    return false;
  }
  if (sign.realSign().isStrictlyPositive()) {
    expr->cloneTreeOverTree(isSum ? 0_e : 1_e);
    return true;
  }
  // sum(k,k,m,n) = n(n+1)/2 - (m-1)m/2
  if (PatternMatching::MatchReplaceSimplify(
          expr, KSum(KA, KB, KC, KVarK),
          KMult(1_e / 2_e, KAdd(KMult(KC, KAdd(1_e, KC)),
                                KMult(-1_e, KB, KAdd(-1_e, KB)))))) {
    return true;
  }
  // sum(k^2,k,m,n) = n(n+1)(2n+1)/6 - (m-1)(m)(2m-1)/6
  if (PatternMatching::MatchReplaceSimplify(
          expr, KSum(KA, KB, KC, KPow(KVarK, 2_e)),
          KMult(KPow(6_e, -1_e),
                KAdd(KMult(KC, KAdd(KC, 1_e), KAdd(KMult(2_e, KC), 1_e)),
                     KMult(-1_e, KAdd(-1_e, KB), KB,
                           KAdd(KMult(2_e, KB), -1_e)))))) {
    return true;
  }
  Tree* child = upperBound->nextTree();
  // HasVariable and HasLocalRandom could be factorized.
  if (Variables::HasVariable(child, k_localVariableId) ||
      HasLocalRandom(expr)) {
    return false;
  }
  // sum(f, k, m, n) = (1+n-m)*f and prod(f, k, m, n) = f^(1 + n - m)
  // TODO: add ceil around bounds
  constexpr KTree numberOfTerms = KAdd(1_e, KA, KMult(-1_e, KB));
  Variables::LeaveScope(child);
  Tree* result = PatternMatching::CreateSimplify(
      isSum ? KMult(numberOfTerms, KC) : KPow(KC, numberOfTerms),
      {.KA = upperBound, .KB = lowerBound, .KC = child});
  expr->moveTreeOverTree(result);
  return true;
}

bool Parametric::ExpandSum(Tree* expr) {
  // sum(f+g,k,a,b) = sum(f,k,a,b) + sum(g,k,a,b)
  // sum(x_k, k, 0, n) = x_0 + ... + x_n
  return expr->isSum() &&
         (PatternMatching::MatchReplaceSimplify(
              expr, KSum(KA, KB, KC, KAdd(KD, KE_p)),
              KAdd(KSum(KA, KB, KC, KD), KSum(KA, KB, KC, KAdd(KE_p)))) ||
          Explicit(expr));
}

bool Parametric::ExpandProduct(Tree* expr) {
  // prod(f*g,k,a,b) = prod(f,k,a,b) * prod(g,k,a,b)
  // prod(x_k, k, 0, n) = x_0 * ... * x_n
  return expr->isProduct() && (PatternMatching::MatchReplaceSimplify(
                                   expr, KProduct(KA, KB, KC, KMult(KD, KE_p)),
                                   KMult(KProduct(KA, KB, KC, KD),
                                         KProduct(KA, KB, KC, KMult(KE_p)))) ||
                               Explicit(expr));
}

/* TODO:
 * - Try swapping sigmas
 * - Different children equal bounds
 * - Identical children where leftUpperBound + 1 = rightLowerBound
 * - Product from/to factorial
 * - Expand and contract distribution with exp/log
 * - Prod(A, B, C, D) / Prod(A, B, F, G) =
 *              Prod(A, B, C, min(F, D)) * Prod(A, B, max(C, G), D)
 *           / Prod(A, B, F, min(G, C)) * Prod(A, B, max(F, D), G)
 *   Same with Sum(A, B, C, D) - Sum(A, B, F, G)
 */

bool Parametric::ContractProduct(Tree* expr) {
  // Used to simplify simplified and projected permute and binomials.
  // Prod(u(k), k, a, b) / Prod(u(k), k, a, c) -> Prod(u(k), k, c+1, b) if c < b
  PatternMatching::Context ctx;
  if (PatternMatching::Match(
          expr,
          KMult(KProduct(KA, KB, KC, KD), KPow(KProduct(KE, KB, KF, KD), -1_e)),
          &ctx) &&
      Comparison::Compare(ctx.getNode(KF), ctx.getNode(KC)) < 0) {
    expr->moveTreeOverTree(PatternMatching::CreateSimplify(
        KProduct(KA, KAdd(KF, 1_e), KC, KD), ctx));
    return true;
  }
  return false;
}

bool Parametric::HasLocalRandom(Tree* expr) {
  return expr->hasDescendantSatisfying(
      [](const Tree* e) { return e->isRandomNode(); });
}

bool Parametric::Explicit(Tree* expr) {
  if (!(expr->isSum() || expr->isProduct())) {
    return false;
  }
  if (HasLocalRandom(expr)) {
    return false;
  }
  bool isSum = expr->isSum();
  const Tree* lowerBound = expr->child(k_lowerBoundIndex);
  const Tree* upperBound = lowerBound->nextTree();
  const Tree* child = upperBound->nextTree();
  Tree* boundsDifference = PatternMatching::CreateSimplify(
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
    Tree* value = SharedTreeStack->push<Type::Add>(2);
    lowerBound->clone();
    Integer::Push(step);
    Simplification::ShallowSystematicReduce(value);
    // Clone the child and replace k with its value
    Tree* clone = child->clone();
    Variables::Replace(clone, k_localVariableId, value, true);
    value->removeTree();
    result->cloneNodeAtNode(isSum ? KAdd.node<2> : KMult.node<2>);
    // Terms are simplified one at a time to avoid overflowing the pool
    Simplification::ShallowSystematicReduce(result);
  }
  expr->moveTreeOverTree(result);
  return true;
}

}  // namespace Poincare::Internal
