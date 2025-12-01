#include "parametric.h"

#include <omg/unreachable.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include "advanced_reduction.h"
#include "dependency.h"
#include "integer.h"
#include "k_tree.h"
#include "matrix.h"
#include "properties.h"
#include "systematic_reduction.h"
#include "undefined.h"
#include "units/unit.h"
#include "variables.h"

namespace Poincare::Internal {

uint8_t Parametric::FunctionIndex(const Tree* e) {
  return FunctionIndex(e->type());
}

uint8_t Parametric::FunctionIndex(TypeBlock type) {
  switch (type) {
    case Type::ListSequence:
    case Type::ListSequenceLayout:
      return 2;
    case Type::Diff:
    case Type::DiffLayout:
    case Type::Integral:
    case Type::IntegralLayout:
    case Type::IntegralWithAlternatives:
    case Type::Sum:
    case Type::SumLayout:
    case Type::Product:
    case Type::ProductLayout:
      return k_integrandIndex;
    default:
      OMG::unreachable();
  }
}

bool Parametric::IsFunctionIndex(int i, const Tree* e) {
  if (e->isIntegralWithAlternatives()) {
    return i >= 3;
  }
  return i == FunctionIndex(e);
}

ComplexProperties Parametric::VariableProperties(const Tree* e) {
  switch (e->type()) {
    case Type::Diff:
    case Type::Integral:
      return k_continuousVariableProperties;
    case Type::ListSequence:
    case Type::Sum:
    case Type::Product:
      return k_discreteVariableProperties;
    default:
      OMG::unreachable();
  }
}

Tree* ReduceSumOrProductAux(const Tree* e, const Tree* lowerBound,
                            const Tree* upperBound, const Tree* function,
                            ComplexProperties properties) {
  assert(e->isSum() || e->isProduct());
  bool isSum = e->isSum();

  // If a > b: sum(f(k),k,a,b) = 0 and prod(f(k),k,a,b) = 1
  if (properties.realSign().isStrictlyNegative()) {
    Dimension dim = Dimension::Get(function);
    Tree* result;
    if (dim.isMatrix()) {
      result = isSum ? Matrix::Zero(dim.matrix) : Matrix::Identity(dim.matrix);
    } else if (dim.isUnit()) {
      // Preserve unit
      result = SharedTreeStack->pushMult(2);
      (0_e)->cloneTree();
      Units::Unit::GetBaseUnits(dim.unit.vector);
      SystematicReduction::ShallowReduce(result);
    } else {
      assert(dim.isScalar());
      result = (isSum ? 0_e : 1_e)->cloneTree();
    }
    return result;
  }

  // If a=b, no need to wait for advanced reduction to explicit
  if (properties.realSign().isNull()) {
    Tree* result = e->cloneTree();
    if (Parametric::Explicit(result)) {
      return result;
    }
    result->removeTree();
    return nullptr;
  }

  // sum(k,k,m,n) = n(n+1)/2 - (m-1)m/2
  Tree* result = PatternMatching::MatchCreateReduce(
      e, KSum(KA, KB, KC, KVarK),
      KMult(1_e / 2_e,
            KAdd(KMult(KC, KAdd(1_e, KC)), KMult(-1_e, KB, KAdd(-1_e, KB)))));
  if (result) {
    return result;
  }

  // sum(k^2,k,m,n) = n(n+1)(2n+1)/6 - (m-1)(m)(2m-1)/6
  result = PatternMatching::MatchCreateReduce(
      e, KSum(KA, KB, KC, KPow(KVarK, 2_e)),
      KMult(KPow(6_e, -1_e),
            KAdd(KMult(KC, KAdd(KC, 1_e), KAdd(KMult(2_e, KC), 1_e)),
                 KMult(-1_e, KAdd(-1_e, KB), KB, KAdd(KMult(2_e, KB), -1_e)))));
  if (result) {
    return result;
  }

  if (Random::HasRandom(e)) {
    return nullptr;
  }

  // sum(f,k,m,n) = (1+n-m)*f and prod(f,k,m,n) = f^(1+n-m)
  if (!Variables::HasVariable(function, Parametric::k_localVariableId)) {
    // TODO: add ceil around bounds
    constexpr SimpleKTrees::KTree numberOfTerms =
        KAdd(1_e, KA, KMult(-1_e, KB));
    Tree* result = function->cloneTree();
    Variables::LeaveScope(result);
    result->moveTreeOverTree(PatternMatching::CreateReduce(
        isSum ? KMult(numberOfTerms, KC) : KPow(KC, numberOfTerms),
        {.KA = upperBound, .KB = lowerBound, .KC = result}));
    return result;
  }

  // sum(a*f(k),k,m,n) = a*sum(f(k),k,m,n)
  if (isSum && function->isMult()) {
    TreeRef result(SharedTreeStack->pushMult(1));
    Tree* sum = e->cloneTree();
    const int nbChildren = function->numberOfChildren();
    int nbChildrenRemoved = 0;
    Tree* function = sum->child(Parametric::k_integrandIndex);
    Tree* child = function->child(0);
    for (int i = 0; i < nbChildren; i++) {
      if (!Variables::HasVariable(child, Parametric::k_localVariableId)) {
        Variables::LeaveScope(child);
        child->detachTree();
        nbChildrenRemoved++;
      } else {
        child = child->nextTree();
      }
    }
    NAry::SetNumberOfChildren(function, nbChildren - nbChildrenRemoved);
    NAry::SetNumberOfChildren(result, nbChildrenRemoved + 1);
    if (nbChildrenRemoved == 0) {
      SharedTreeStack->flushFromBlock(result);
      return nullptr;
    }
    assert(function->numberOfChildren() > 0);  // Because HasVariable
    NAry::SquashIfUnary(function);
    assert(!SystematicReduction::ShallowReduce(function));
    // Shallow reduce the Sum
    SystematicReduction::ShallowReduce(sum);
    // Shallow reduce a*Sum
    SystematicReduction::ShallowReduce(result);
    return result;
  }

  // prod(f(k)^a,k,m,n) = prod(f(k),k,m,n)^a
  if (!isSum && function->isPow()) {
    Tree* result = e->cloneTree();
    assert(function->child(1)->isInteger());
    // If a wasn't an integer, we would need to add Variables::LeaveScope(a)
    assert(!Variables::HasVariable(function->child(1),
                                   Parametric::k_localVariableId));
    // Move the node Pow before the Prod
    result->moveNodeAtNode(result->child(Parametric::k_integrandIndex));
    // Shallow reduce the Prod
    SystematicReduction::ShallowReduce(result->child(0));
    // Shallow reduce Prod^a
    SystematicReduction::ShallowReduce(result);
    return result;
  }
  return nullptr;
}

bool Parametric::ReduceSumOrProduct(Tree* e) {
  const Tree* lowerBound = e->child(k_lowerBoundIndex);
  const Tree* upperBound = lowerBound->nextTree();
  constexpr bool (*HasUserSymbol)(const Tree*) = [](const Tree* e) {
    return e->hasDescendantSatisfying(
        [](const Tree* child) { return child->isUserSymbol(); });
  };
  ComplexProperties propertiesLowerBound = GetComplexProperties(lowerBound);
  ComplexProperties propertiesUpperBound = GetComplexProperties(upperBound);
  if (propertiesLowerBound.isNonReal() || propertiesUpperBound.isNonReal()) {
    // NOTE: catches bounds like: i, k+i
    Undefined::ReplaceTreeWithDimensionedType(e, Type::UndefOutOfDefinition);
    return true;
  }
  ComplexProperties propertiesDifference =
      ComplexPropertiesOfDifference(upperBound, lowerBound);
  /* Since child should be reduced at this point, ensure bounds are integer or
   * contains UserSymbol (CAS) */
  if (!propertiesLowerBound.isInteger() || !propertiesUpperBound.isInteger()) {
    bool boundsHaveSymbol =
        HasUserSymbol(lowerBound) || HasUserSymbol(upperBound);
    if (!boundsHaveSymbol || propertiesDifference.isNonReal()) {
      // NOTE: catches bounds like: π
      Undefined::ReplaceTreeWithDimensionedType(e, Type::UndefOutOfDefinition);
      return true;
    }
    if (!propertiesDifference.isReal()) {
      // NOTE: catches bounds like: k×i
      return false;
    }
    assert(boundsHaveSymbol && propertiesDifference.isReal());
    // TODO: Introduce piecewise for inverted bounds:
    // sum(...,k,j) => {sum(...,k,j) if k<=j else 0}
    // NOTE: Dependencies will catch bounds like: k+π
  }

  const Tree* function = upperBound->nextTree();
  static_assert(k_lowerBoundIndex + 1 == k_upperBoundIndex &&
                k_upperBoundIndex + 1 == k_integrandIndex);
  TreeRef result = ReduceSumOrProductAux(e, lowerBound, upperBound, function,
                                         propertiesDifference);
  if (!result) {
    return false;
  }
  // TODO: Skip the dependency if propertiesLowerBound+propertiesUpperBound are
  // `isInteger`
  e->moveTreeOverTree(PatternMatching::CreateReduce(
      KDep(KA, KDepList(KRealInteger(KB), KRealInteger(KC))),
      {.KA = result, .KB = lowerBound, .KC = upperBound}));
  result->removeTree();
  Dependency::ShallowRemoveUselessDependencies(e);
  return true;
}

bool Parametric::ExpandSum(Tree* e) {
  return e->isSum() &&
         // sum(f+g,k,a,b) = sum(f,k,a,b) + sum(g,k,a,b)
         (PatternMatching::MatchReplaceReduce(
              e, KSum(KA, KB, KC, KAdd(KD, KE_p)),
              KAdd(KSum(KA, KB, KC, KD), KSum(KA, KB, KC, KAdd(KE_p)))) ||
          // sum(x_k, k, 0, n) = x_0 + ... + x_n
          Explicit(e));
}

bool Parametric::ExpandProduct(Tree* e) {
  return e->isProduct() &&
         // prod(f*g,k,a,b) = prod(f,k,a,b) * prod(g,k,a,b)
         (PatternMatching::MatchReplaceReduce(
              e, KProduct(KA, KB, KC, KMult(KD, KE_p)),
              KMult(KProduct(KA, KB, KC, KD),
                    KProduct(KA, KB, KC, KMult(KE_p)))) ||
          // prod(x_k, k, 0, n) = x_0 * ... * x_n
          Explicit(e));
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

bool Parametric::ContractSum(Tree* e) {
  /* TODO: handle any form:
   * - KAdd(KA_s, KSum, KB_s, KMult(KSum, -1_e), KC_s)
   * - KAdd(KA_s, KMult(KSum, -1_e), KB_s, KSum, KC_s) */
  // Used to reduce simplified and projected permute and binomials.
  /* Sum(u(k), k, a, b) - Sum(u(k), k, a, c)
   * -> Sum(u(k), k, c+1, b) if b > c
   * -> 0 if b = c (can be included in previous case and then will be reduced)
   * -> - Sum(u(k), k, b+1, c) if b < c */
  PatternMatching::Context ctx;
  if (PatternMatching::Match(
          e, KAdd(KSum(KA, KB, KC, KD), KMult(-1_e, KSum(KE, KB, KF, KD))),
          &ctx)) {
    ComplexProperties properties =
        ComplexPropertiesOfDifference(ctx.getTree(KC), ctx.getTree(KF));
    Sign realSign = properties.realSign();
    if (properties.isReal() &&
        (realSign.isNull() || realSign.isStrictlyPositive() ||
         realSign.isStrictlyNegative())) {
      Tree* result =
          realSign.isNull() || realSign.isStrictlyPositive()
              ? PatternMatching::CreateReduce(
                    KDep(KSum(KA, KAdd(KF, 1_e), KC, KD),
                         KDepList(KRealInteger(KB))),
                    ctx)
              : PatternMatching::CreateReduce(
                    KDep(KMult(KSum(KE, KAdd(KC, 1_e), KF, KD), -1_e),
                         KDepList(KRealInteger(KB))),
                    ctx);
      e->moveTreeOverTree(result);
      return true;
    }
  }
  /* Sum(u(k), k, a, c) - Sum(u(k), k, b, c)
   * -> Sum(u(k), k, a, b-1) if a < b
   * -> 0 if a = b (can be included in previous case and then will be reduced)
   * -> - Sum(u(k), k, b, a-1) if a > b */
  if (PatternMatching::Match(
          e, KAdd(KSum(KA, KB, KC, KD), KMult(-1_e, KSum(KE, KF, KC, KD))),
          &ctx)) {
    ComplexProperties properties =
        ComplexPropertiesOfDifference(ctx.getTree(KB), ctx.getTree(KF));
    Sign realSign = properties.realSign();
    if (properties.isReal() &&
        (realSign.isNull() || realSign.isStrictlyPositive() ||
         realSign.isStrictlyNegative())) {
      Tree* result =
          realSign.isNull() || realSign.isStrictlyNegative()
              ? PatternMatching::CreateReduce(
                    KDep(KSum(KA, KB, KAdd(KF, -1_e), KD),
                         KDepList(KRealInteger(KC))),
                    ctx)
              : PatternMatching::CreateReduce(
                    KDep(KMult(KSum(KE, KF, KAdd(KB, -1_e), KD), -1_e),
                         KDepList(KRealInteger(KC))),
                    ctx);

      e->moveTreeOverTree(result);
      return true;
    }
  }
  return false;
}

bool Parametric::ContractProduct(Tree* e) {
  /* TODO: handle any form:
   * - KMult(KA_s, KProduct, KB_s, KPow(KProduct, -1_e), KC_s)
   * - KMult(KA_s, KPow(KProduct, -1_e), KB_s, KProduct, KC_s) */
  // Used to reduce simplified and projected permute and binomials.
  /* Prod(u(k), k, a, b) / Prod(u(k), k, a, c)
   * -> Prod(u(k), k, c+1, b) if b > c
   * -> 1 if b = c (can be included in previous case and then will be reduced)
   * -> 1 / Prod(u(k), k, b+1, c) if b < c */
  PatternMatching::Context ctx;
  if (PatternMatching::Match(
          e,
          KMult(KProduct(KA, KB, KC, KD), KPow(KProduct(KE, KB, KF, KD), -1_e)),
          &ctx)) {
    ComplexProperties properties =
        ComplexPropertiesOfDifference(ctx.getTree(KC), ctx.getTree(KF));
    Sign realSign = properties.realSign();
    if (properties.isReal() &&
        (realSign.isNull() || realSign.isStrictlyPositive() ||
         realSign.isStrictlyNegative())) {
      Tree* result =
          realSign.isNull() || realSign.isStrictlyPositive()
              ? PatternMatching::CreateReduce(
                    KDep(KProduct(KA, KAdd(KF, 1_e), KC, KD),
                         KDepList(KRealInteger(KB))),
                    ctx)
              : PatternMatching::CreateReduce(
                    KDep(KPow(KProduct(KE, KAdd(KC, 1_e), KF, KD), -1_e),
                         KDepList(KRealInteger(KB))),
                    ctx);
      e->moveTreeOverTree(result);
      return true;
    }
  }
  /* Prod(u(k), k, a, c) / Prod(u(k), k, b, c)
   * -> Prod(u(k), k, a, b-1) if a < b
   * -> 1 if a = b (can be included in previous case and then will be reduced)
   * -> 1 / Prod(u(k), k, b, a-1) if a > b */
  if (PatternMatching::Match(
          e,
          KMult(KProduct(KA, KB, KC, KD), KPow(KProduct(KE, KF, KC, KD), -1_e)),
          &ctx)) {
    ComplexProperties properties =
        ComplexPropertiesOfDifference(ctx.getTree(KB), ctx.getTree(KF));
    Sign realSign = properties.realSign();
    if (properties.isReal() &&
        (realSign.isNull() || realSign.isStrictlyPositive() ||
         realSign.isStrictlyNegative())) {
      Tree* result =
          realSign.isNull() || realSign.isStrictlyNegative()
              ? PatternMatching::CreateReduce(
                    KDep(KProduct(KA, KB, KAdd(KF, -1_e), KD),
                         KDepList(KRealInteger(KC))),
                    ctx)
              : PatternMatching::CreateReduce(
                    KDep(KPow(KProduct(KE, KF, KAdd(KB, -1_e), KD), -1_e),
                         KDepList(KRealInteger(KC))),
                    ctx);
      e->moveTreeOverTree(result);
      return true;
    }
  }
  return false;
}

bool Parametric::Explicit(Tree* e) {
  if (!(e->isSum() || e->isProduct())) {
    return false;
  }
  if (Random::HasRandom(e)) {
    return false;
  }
  bool isSum = e->isSum();
  const Tree* lowerBound = e->child(k_lowerBoundIndex);
  const Tree* upperBound = lowerBound->nextTree();
  const Tree* child = upperBound->nextTree();
  Tree* numberOfTerms = PatternMatching::CreateReduce(
      KAdd(KA, KMult(-1_e, KB), 1_e), {.KA = upperBound, .KB = lowerBound});
  Dependency::RemoveDependencies(numberOfTerms);
  AdvancedReduction::DeepExpandAlgebraic(numberOfTerms);
  Dependency::RemoveDependencies(numberOfTerms);
  if (!Integer::Is<uint16_t>(numberOfTerms)) {
    numberOfTerms->removeTree();
    return false;
  }
  uint16_t nbTerms = Integer::Handler(numberOfTerms).to<uint16_t>();
  numberOfTerms->removeTree();
  Tree* result;
  if (isSum) {
    Dimension d = Dimension::Get(child);
    result = d.isMatrix() ? Matrix::Zero(d.matrix) : (0_e)->cloneTree();
  } else {
    result = (1_e)->cloneTree();
  }
  for (uint16_t step = 0; step < nbTerms; step++) {
    // Create k value at this step
    Tree* value = SharedTreeStack->pushAdd(2);
    lowerBound->cloneTree();
    Integer::Push(step);
    SystematicReduction::ShallowReduce(value);
    // Clone the child and replace k with its value
    Tree* clone = child->cloneTree();
    Variables::Replace(clone, k_localVariableId, value, true);
    value->removeTree();
    result->cloneNodeAtNode(isSum ? KAdd.node<2> : KMult.node<2>);
    // Terms are simplified one at a time to avoid overflowing the pool
    SystematicReduction::ShallowReduce(result);
  }
  e->moveTreeOverTree(result);
  return true;
}

bool Parametric::ExpandExpOfSum(Tree* e) {
  // TODO: factorize with AdvancedOperation::ExpandExp
  // exp(a*sum(f(k),k,m,n)*b) = product(exp(a*f(k)*b),k,m,n)
  PatternMatching::Context ctx;
  if (PatternMatching::Match(e, KExp(KMult(KA_s, KSum(KB, KC, KD, KE), KF_s)),
                             &ctx)) {
    // KA and KF need to enter KProduct scope
    TreeRef a = PatternMatching::Create(KMult(KA_s), ctx);
    Variables::EnterScope(a);
    ctx.setNode(KA, a, 1, false, 1);
    TreeRef f = PatternMatching::Create(KMult(KF_s), ctx);
    Variables::EnterScope(f);
    ctx.setNode(KF, f, 1, false, 1);
    e->moveTreeOverTree(PatternMatching::CreateReduce(
        KProduct(KB, KC, KD, KExp(KMult(KA, KE, KF))), ctx));
    SharedTreeStack->flushFromBlock(a);  // Removes a and f
    return true;
  }
  return false;
}

bool Parametric::ContractProductOfExp(Tree* e) {
  // TODO: factorize with AdvancedOperation::ContractExp
  // product(exp(f(k)),k,m,n) = exp(sum(f(k),k,m,n))
  return PatternMatching::MatchReplaceReduce(e, KProduct(KA, KB, KC, KExp(KD)),
                                             KExp(KSum(KA, KB, KC, KD)));
}

}  // namespace Poincare::Internal
