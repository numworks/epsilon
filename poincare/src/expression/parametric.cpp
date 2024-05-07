#include "parametric.h"

#include <poincare/src/memory/n_ary.h>
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
  assert(expr->isSum() || expr->isProduct());
  bool isSum = expr->isSum();
  Tree* lowerBound = expr->child(k_lowerBoundIndex);
  Tree* upperBound = lowerBound->nextTree();
  ComplexSign sign = ComplexSign::SignOfDifference(lowerBound, upperBound);
  if (!sign.isReal()) {
    return false;
  }

  // If a > b: sum(f(k),k,a,b) = 0 and prod(f(k),k,a,b) = 1
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

  if (HasLocalRandom(expr)) {
    return false;
  }

  Tree* function = upperBound->nextTree();
  bool functionDependsOnK = Variables::HasVariable(function, k_localVariableId);

  // sum(a*f(k),k,m,n) = a*sum(f(k),k,m,n)
  if (isSum && function->isMult() && functionDependsOnK) {
    TreeRef a(SharedTreeStack->push<Type::Mult>(0));
    const int nbChildren = function->numberOfChildren();
    int nbChildrenRemoved = 0;
    Tree* child = function->firstChild();
    for (int i = 0; i < nbChildren; i++) {
      if (!Variables::HasVariable(child, k_localVariableId)) {
        int realI = i - nbChildrenRemoved;
        Tree* t = NAry::DetachChildAtIndex(function, realI);
        NAry::AddChild(a, t);
        Variables::LeaveScope(t);
        nbChildrenRemoved++;
      } else if (i < nbChildren - 1) {
        child = child->nextTree();
      }
    }
    if (a->numberOfChildren() == 0) {
      return false;
    }
    assert(function->numberOfChildren() > 0);  // Because functionDependsOnK
    if (function->numberOfChildren() == 1) {
      // Shallow reduce to remove the Mult
      Simplification::ShallowSystematicReduce(function);
    }
    // Shallow reduce the Sum
    Simplification::ShallowSystematicReduce(expr);
    // Add factor a before the Sum
    expr->moveTreeBeforeNode(a);
    // a is already a Mult, increase its number of children to include the Sum
    NAry::SetNumberOfChildren(expr, expr->numberOfChildren() + 1);
    // Shallow reduce a*Sum
    Simplification::ShallowSystematicReduce(expr);
    return true;
  }

  // prod(f(k)^a,k,m,n) = prod(f(k),k,m,n)^a
  if (!isSum && function->isPow()) {
    Tree* a = function->child(1);
    assert(a->isInteger());
    assert(!Variables::HasVariable(a, k_localVariableId));
    Variables::LeaveScope(a);
    // Move the node Pow before the Prod
    expr->moveNodeBeforeNode(function);
    // Shallow reduce the Prod
    Simplification::ShallowSystematicReduce(expr->firstChild());
    // Shallow reduce Prod^a
    Simplification::ShallowSystematicReduce(expr);
    return true;
  }

  // sum(f,k,m,n) = (1+n-m)*f and prod(f,k,m,n) = f^(1+n-m)
  // TODO: add ceil around bounds
  if (functionDependsOnK) {
    return false;
  }
  constexpr KTree numberOfTerms = KAdd(1_e, KA, KMult(-1_e, KB));
  Variables::LeaveScope(function);
  Tree* result = PatternMatching::CreateSimplify(
      isSum ? KMult(numberOfTerms, KC) : KPow(KC, numberOfTerms),
      {.KA = upperBound, .KB = lowerBound, .KC = function});
  expr->moveTreeOverTree(result);
  return true;
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
 */

bool Parametric::ContractProduct(Tree* expr) {
  // Used to simplify simplified and projected permute and binomials.
  // Prod(u(k), k, a, b) / Prod(u(k), k, a, c) -> Prod(u(k), k, c+1, b) if c < b
  PatternMatching::Context ctx;
  if (PatternMatching::Match(
          KMult(KProduct(KA, KB, KC, KD), KPow(KProduct(KE, KB, KF, KD), -1_e)),
          expr, &ctx) &&
      Comparison::Compare(ctx.getNode(KF), ctx.getNode(KC)) < 0) {
    expr->moveTreeOverTree(PatternMatching::CreateSimplify(
        KProduct(KA, KAdd(KF, 1_e), KC, KD), ctx));
    return true;
  }
  return false;
}

bool Parametric::HasLocalRandom(Tree* expr) {
  // TODO: could be factorized with HasVariable
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
  // TODO: larger type than uint8
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

bool Parametric::ExpandExpOfSum(Tree* expr) {
  // TODO: factorise with AdvancedOperation::ExpandExp
  // exp(a*sum(f(k),k,m,n)) = product(exp(a*f(k)),k,m,n)
  return PatternMatching::MatchReplaceSimplify(
      expr, KExp(KMult(KA_s, KSum(KB, KC, KD, KE))),
      KProduct(KB, KC, KD, KExp(KMult(KA_s, KE))));
}

bool Parametric::ContractProductOfExp(Tree* expr) {
  // TODO: factorise with AdvancedOperation::ContractExp
  // product(exp(f(k)),k,m,n) = exp(sum(f(k),k,m,n))
  return PatternMatching::MatchReplaceSimplify(
      expr, KProduct(KA, KB, KC, KExp(KD)), KExp(KSum(KA, KB, KC, KD)));
}

}  // namespace Poincare::Internal
