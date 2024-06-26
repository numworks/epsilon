#include "parametric.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "advanced_simplification.h"
#include "integer.h"
#include "k_tree.h"
#include "matrix.h"
#include "sign.h"
#include "systematic_reduction.h"
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

bool Parametric::IsFunctionIndex(int i, const Tree* t) {
  if (t->isIntegralWithAlternatives()) {
    return i >= 3;
  }
  return i == FunctionIndex(t);
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
  assert(expr->isSum() || expr->isProduct());
  bool isSum = expr->isSum();
  Tree* lowerBound = expr->child(k_lowerBoundIndex);
  Tree* upperBound = lowerBound->nextTree();
  ComplexSign sign = ComplexSign::SignOfDifference(lowerBound, upperBound);
  // TODO: what about when bounds are not integer but diff is integer ?
  if (!sign.isReal()) {
    return false;
  }

  // If a > b: sum(f(k),k,a,b) = 0 and prod(f(k),k,a,b) = 1
  if (sign.realSign().isStrictlyPositive()) {
    expr->cloneTreeOverTree(isSum ? 0_e : 1_e);
    return true;
  }

  // If a=b, no need to wait for advanced reduction to explicit
  if (sign.realSign().isZero()) {
    return Explicit(expr);
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

  if (HasLocalRandom(expr)) {
    return false;
  }

  Tree* function = upperBound->nextTree();

  // sum(f,k,m,n) = (1+n-m)*f and prod(f,k,m,n) = f^(1+n-m)
  if (!Variables::HasVariable(function, k_localVariableId)) {
    // TODO: add ceil around bounds
    constexpr KTree numberOfTerms = KAdd(1_e, KA, KMult(-1_e, KB));
    Variables::LeaveScope(function);
    Tree* result = PatternMatching::CreateSimplify(
        isSum ? KMult(numberOfTerms, KC) : KPow(KC, numberOfTerms),
        {.KA = upperBound, .KB = lowerBound, .KC = function});
    expr->moveTreeOverTree(result);
    return true;
  }

  // sum(a*f(k),k,m,n) = a*sum(f(k),k,m,n)
  if (isSum && function->isMult()) {
    TreeRef a(SharedTreeStack->pushMult(0));
    const int nbChildren = function->numberOfChildren();
    int nbChildrenRemoved = 0;
    Tree* child = function->child(0);
    for (int i = 0; i < nbChildren; i++) {
      if (!Variables::HasVariable(child, k_localVariableId)) {
        Variables::LeaveScope(child);
        child->detachTree();
        nbChildrenRemoved++;
      } else {
        child = child->nextTree();
      }
    }
    NAry::SetNumberOfChildren(function, nbChildren - nbChildrenRemoved);
    NAry::SetNumberOfChildren(a, nbChildrenRemoved);
    if (a->numberOfChildren() == 0) {
      a->removeTree();
      return false;
    }
    assert(function->numberOfChildren() > 0);  // Because HasVariable
    if (function->numberOfChildren() == 1) {
      // Shallow reduce to remove the Mult
      SystematicReduction::ShallowSystematicReduce(function);
    }
    // Shallow reduce the Sum
    SystematicReduction::ShallowSystematicReduce(expr);
    // Add factor a before the Sum
    expr->moveTreeBeforeNode(a);
    // a is already a Mult, increase its number of children to include the Sum
    NAry::SetNumberOfChildren(expr, expr->numberOfChildren() + 1);
    // Shallow reduce a*Sum
    SystematicReduction::ShallowSystematicReduce(expr);
    return true;
  }

  // prod(f(k)^a,k,m,n) = prod(f(k),k,m,n)^a
  if (!isSum && function->isPow()) {
    Tree* a = function->child(1);
    assert(a->isInteger());
    // If a wasn't an integer, we would need to add Variables::LeaveScope(a)
    assert(!Variables::HasVariable(a, k_localVariableId));
    // Move the node Pow before the Prod
    expr->moveNodeBeforeNode(function);
    // Shallow reduce the Prod
    SystematicReduction::ShallowSystematicReduce(expr->child(0));
    // Shallow reduce Prod^a
    SystematicReduction::ShallowSystematicReduce(expr);
    return true;
  }

  return false;
}

bool Parametric::ExpandSum(Tree* expr) {
  return expr->isSum() &&
         // sum(f+g,k,a,b) = sum(f,k,a,b) + sum(g,k,a,b)
         (PatternMatching::MatchReplaceSimplify(
              expr, KSum(KA, KB, KC, KAdd(KD, KE_p)),
              KAdd(KSum(KA, KB, KC, KD), KSum(KA, KB, KC, KAdd(KE_p)))) ||
          // sum(x_k, k, 0, n) = x_0 + ... + x_n
          Explicit(expr));
}

bool Parametric::ExpandProduct(Tree* expr) {
  return expr->isProduct() &&
         // prod(f*g,k,a,b) = prod(f,k,a,b) * prod(g,k,a,b)
         (PatternMatching::MatchReplaceSimplify(
              expr, KProduct(KA, KB, KC, KMult(KD, KE_p)),
              KMult(KProduct(KA, KB, KC, KD),
                    KProduct(KA, KB, KC, KMult(KE_p)))) ||
          // prod(x_k, k, 0, n) = x_0 * ... * x_n
          Explicit(expr));
}

/* TODO:
 * - Try swapping sums (same with prods)
 * - int(f(k),k,a,a) = 0
 * - Product from/to factorial
 * - sum(ln(f(k))) = ln(prod(f(k))) but not that simple (cf
 *   Logarithm::ContractLn and Logarithm::ExpandLn)
 * - Prod(A, B, C, D) / Prod(A, B, F, G) =
 *              Prod(A, B, C, min(F, D)) * Prod(A, B, max(C, G), D)
 *           / Prod(A, B, F, min(G, C)) * Prod(A, B, max(F, D), G)
 *   Same with Sum(A, B, C, D) - Sum(A, B, F, G)
 * - sum(u(k), k, a, b) + sum(v(k), k, a, b) =
 *   sum(u(k), k, min(a,d), min(c,b)) +
 *   sum(u(k), k, max(a,d), max(c,b)) +
 *   sum(u(k)+v(k), k, max(a,c), min(b,d)) +
 *   sum(v(k), k, min(c,b), min(a,d)) +
 *   sum(v(k), k, min(c,b), max(a,d))
 */

bool Parametric::ContractSum(Tree* expr) {
  /* TODO: handle any form:
   * - KAdd(KA_s, KSum, KB_s, KMult(KSum, -1_e), KC_s)
   * - KAdd(KA_s, KMult(KSum, -1_e), KB_s, KSum, KC_s) */
  // Used to simplify simplified and projected permute and binomials.
  /* Sum(u(k), k, a, b) - Sum(u(k), k, a, c)
   * -> Sum(u(k), k, c+1, b) if b > c
   * -> 0 if b = c (can be included in previous case and then will be reduced)
   * -> - Sum(u(k), k, b+1, c) if b < c */
  PatternMatching::Context ctx;
  if (PatternMatching::Match(
          KAdd(KSum(KA, KB, KC, KD), KMult(-1_e, KSum(KE, KB, KF, KD))), expr,
          &ctx)) {
    ComplexSign sign =
        ComplexSign::SignOfDifference(ctx.getTree(KC), ctx.getTree(KF));
    Sign realSign = sign.realSign();
    if (sign.isReal() && (realSign.isZero() || realSign.isStrictlyPositive() ||
                          realSign.isStrictlyNegative())) {
      Tree* result =
          realSign.isZero() || realSign.isStrictlyPositive()
              ? PatternMatching::CreateSimplify(KSum(KA, KAdd(KF, 1_e), KC, KD),
                                                ctx)
              : PatternMatching::CreateSimplify(
                    KMult(KSum(KE, KAdd(KC, 1_e), KF, KD), -1_e), ctx);
      expr->moveTreeOverTree(result);
      return true;
    }
  }
  /* Sum(u(k), k, a, c) - Sum(u(k), k, b, c)
   * -> Sum(u(k), k, a, b-1) if a < b
   * -> 0 if a = b (can be included in previous case and then will be reduced)
   * -> - Sum(u(k), k, b, a-1) if a > b */
  if (PatternMatching::Match(
          KAdd(KSum(KA, KB, KC, KD), KMult(-1_e, KSum(KE, KF, KC, KD))), expr,
          &ctx)) {
    ComplexSign sign =
        ComplexSign::SignOfDifference(ctx.getTree(KB), ctx.getTree(KF));
    Sign realSign = sign.realSign();
    if (sign.isReal() && (realSign.isZero() || realSign.isStrictlyPositive() ||
                          realSign.isStrictlyNegative())) {
      Tree* result =
          realSign.isZero() || realSign.isStrictlyNegative()
              ? PatternMatching::CreateSimplify(
                    KSum(KA, KB, KAdd(KF, -1_e), KD), ctx)
              : PatternMatching::CreateSimplify(
                    KMult(KSum(KE, KF, KAdd(KB, -1_e), KD), -1_e), ctx);
      expr->moveTreeOverTree(result);
      return true;
    }
  }
  return false;
}

bool Parametric::ContractProduct(Tree* expr) {
  /* TODO: handle any form:
   * - KMult(KA_s, KProduct, KB_s, KPow(KProduct, -1_e), KC_s)
   * - KMult(KA_s, KPow(KProduct, -1_e), KB_s, KProduct, KC_s) */
  // Used to simplify simplified and projected permute and binomials.
  /* Prod(u(k), k, a, b) / Prod(u(k), k, a, c)
   * -> Prod(u(k), k, c+1, b) if b > c
   * -> 1 if b = c (can be included in previous case and then will be reduced)
   * -> 1 / Prod(u(k), k, b+1, c) if b < c */
  PatternMatching::Context ctx;
  if (PatternMatching::Match(
          KMult(KProduct(KA, KB, KC, KD), KPow(KProduct(KE, KB, KF, KD), -1_e)),
          expr, &ctx)) {
    ComplexSign sign =
        ComplexSign::SignOfDifference(ctx.getTree(KC), ctx.getTree(KF));
    Sign realSign = sign.realSign();
    if (sign.isReal() && (realSign.isZero() || realSign.isStrictlyPositive() ||
                          realSign.isStrictlyNegative())) {
      Tree* result =
          realSign.isZero() || realSign.isStrictlyPositive()
              ? PatternMatching::CreateSimplify(
                    KProduct(KA, KAdd(KF, 1_e), KC, KD), ctx)
              : PatternMatching::CreateSimplify(
                    KPow(KProduct(KE, KAdd(KC, 1_e), KF, KD), -1_e), ctx);
      expr->moveTreeOverTree(result);
      return true;
    }
  }
  /* Prod(u(k), k, a, c) / Prod(u(k), k, b, c)
   * -> Prod(u(k), k, a, b-1) if a < b
   * -> 1 if a = b (can be included in previous case and then will be reduced)
   * -> 1 / Prod(u(k), k, b, a-1) if a > b */
  if (PatternMatching::Match(
          KMult(KProduct(KA, KB, KC, KD), KPow(KProduct(KE, KF, KC, KD), -1_e)),
          expr, &ctx)) {
    ComplexSign sign =
        ComplexSign::SignOfDifference(ctx.getTree(KB), ctx.getTree(KF));
    Sign realSign = sign.realSign();
    if (sign.isReal() && (realSign.isZero() || realSign.isStrictlyPositive() ||
                          realSign.isStrictlyNegative())) {
      Tree* result =
          realSign.isZero() || realSign.isStrictlyNegative()
              ? PatternMatching::CreateSimplify(
                    KProduct(KA, KB, KAdd(KF, -1_e), KD), ctx)
              : PatternMatching::CreateSimplify(
                    KPow(KProduct(KE, KF, KAdd(KB, -1_e), KD), -1_e), ctx);
      expr->moveTreeOverTree(result);
      return true;
    }
  }
  return false;
}

bool Parametric::HasLocalRandom(const Tree* expr) {
  // TODO: could be factorized with HasVariable
  return expr->hasDescendantSatisfying(
      [](const Tree* e) { return e->isRandomized(); });
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
  AdvancedSimplification::DeepExpand(boundsDifference);
  // TODO: larger type than uint8
  if (!Integer::Is<uint8_t>(boundsDifference)) {
    boundsDifference->removeTree();
    return false;
  }
  uint8_t numberOfTerms = Integer::Handler(boundsDifference).to<uint8_t>() + 1;
  boundsDifference->removeTree();
  Tree* result;
  if (isSum) {
    Dimension d = Dimension::Get(child);
    result = d.isMatrix() ? Matrix::Zero(d.matrix) : (0_e)->cloneTree();
  } else {
    result = (1_e)->cloneTree();
  }
  for (uint8_t step = 0; step < numberOfTerms; step++) {
    // Create k value at this step
    Tree* value = SharedTreeStack->pushAdd(2);
    lowerBound->cloneTree();
    Integer::Push(step);
    SystematicReduction::ShallowSystematicReduce(value);
    // Clone the child and replace k with its value
    Tree* clone = child->cloneTree();
    Variables::Replace(clone, k_localVariableId, value, true);
    value->removeTree();
    result->cloneNodeAtNode(isSum ? KAdd.node<2> : KMult.node<2>);
    // Terms are simplified one at a time to avoid overflowing the pool
    SystematicReduction::ShallowSystematicReduce(result);
  }
  expr->moveTreeOverTree(result);
  return true;
}

bool Parametric::ExpandExpOfSum(Tree* expr) {
  // TODO: factorize with AdvancedOperation::ExpandExp
  // exp(a*sum(f(k),k,m,n)*b) = product(exp(a*f(k)*b),k,m,n)
  return PatternMatching::MatchReplaceSimplify(
      expr, KExp(KMult(KA_s, KSum(KB, KC, KD, KE), KF_s)),
      KProduct(KB, KC, KD, KExp(KMult(KA_s, KE, KF_s))));
}

bool Parametric::ContractProductOfExp(Tree* expr) {
  // TODO: factorize with AdvancedOperation::ContractExp
  // product(exp(f(k)),k,m,n) = exp(sum(f(k),k,m,n))
  return PatternMatching::MatchReplaceSimplify(
      expr, KProduct(KA, KB, KC, KExp(KD)), KExp(KSum(KA, KB, KC, KD)));
}

}  // namespace Poincare::Internal
