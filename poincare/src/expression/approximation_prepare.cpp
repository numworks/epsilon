#include <poincare/src/memory/pattern_matching.h>
#include <poincare/symbol_context.h>

#include "advanced_reduction.h"
#include "approximation.h"
#include "beautification.h"
#include "dependency.h"
#include "k_tree.h"
#include "random.h"
#include "simplification.h"
#include "systematic_reduction.h"
#include "variables.h"

namespace Poincare::Internal::Approximation {

namespace Private {
bool ShallowPrepareForApproximation(Tree* e, void* ctx) {
  // TODO: we want x^-1 -> 1/x and y*x^-1 -> y/x but maybe not x^-2 -> 1/x^2 ?
  // TODO: Ensure no node is duplicated (random not may have not been seeded)
  // Do exp => pow first to allow pow => sqrt/div to take effect next.
  bool changed =
      // exp(A*ln(B))-> B^A
      PatternMatching::MatchReplace(e, KExp(KMult(KA_s, KLn(KB), KC_s)),
                                    KPow(KB, KMult(KA_s, KC_s))) ||
      // exp(A*π*B*i*C)-> (-1)^(A*B*C), ln(-1) could have been simplified to π*i
      PatternMatching::MatchReplace(e, KExp(KMult(KA_s, π_e, KB_s, i_e, KC_s)),
                                    KPow(-1_e, KMult(KA_s, KB_s, KC_s))) ||
      /* TODO: e^ is better than exp because we have code to handle special
       *       cases in pow, exp is probably more precise on normal values */
      // exp(A) -> e^A
      PatternMatching::MatchReplace(e, KExp(KA), KPow(e_e, KA));
  return
      // A^-1 -> 1/A
      PatternMatching::MatchReplace(e, KPowReal(KA, -1_e), KDiv(1_e, KA)) ||
      PatternMatching::MatchReplace(e, KPow(KA, -1_e), KDiv(1_e, KA)) ||
      // A^0.5 -> sqrt(A)
      /* PowReal(A,0.5) -> dep(sqrt(A), RealPos(A)) has been disabled because it
       * made the expression size grow exponentially in cases of nested square
       * roots. */
      PatternMatching::MatchReplace(e, KPow(KA, 1_e / 2_e), KSqrt(KA)) ||
      //  ln(A)/ln(10) -> log(A)
      PatternMatching::MatchReplace(
          e, KMult(KA_s, KLn(KB), KPow(KLn(10_e), -1_e), KC_s),
          KMult(KA_s, KLog(KB), KC_s)) ||
      PatternMatching::MatchReplace(
          e, KMult(KA_s, KPow(KLn(10_e), -1_e), KLn(KB), KC_s),
          KMult(KA_s, KLog(KB), KC_s)) ||
      changed;
}

static Tree* RewriteIntegrandNear(const Tree* integrand, const Tree* bound) {
  Tree* value = SharedTreeStack->pushAdd(2);
  // Bound could contain variables that needs to be scoped.
  Tree* scopedBound = bound->cloneTree();
  Variables::EnterScope(scopedBound);
  KVarX->cloneTree();
  SystematicReduction::DeepReduce(value);
  Tree* tree = integrand->cloneTree();
  // TODO: This deep should have been done before
  SystematicReduction::DeepReduce(tree);
  Variables::Replace(tree, 0, value, false, true);
  /* We need to remove the constant part by expanding polynomials introduced by
   * the replacement, e.g. 1-(1-x)^2 -> 2x-x^2 */
  AdvancedReduction::DeepExpandAlgebraic(tree);
  value->removeTree();
  return value;
}

static bool ShallowExpandIntegrals(Tree* e, void* ctx) {
  if (!e->isIntegral()) {
    return false;
  }
  const Tree* lowerBound = e->child(Parametric::k_lowerBoundIndex);
  const Tree* upperBound = e->child(Parametric::k_upperBoundIndex);
  const Tree* integrand = e->child(Parametric::k_integrandIndex);
  // Random in bounds are not handled here
  if (Random::HasRandom(lowerBound) || Random::HasRandom(upperBound)) {
    return false;
  }
  Tree* insertAt = e->nextTree();
  // Rewrite the integrand to be able to compute it directly at abscissa b - x
  Tree* upperIntegrand = RewriteIntegrandNear(integrand, upperBound);
  insertAt->moveTreeAtNode(upperIntegrand);
  // Same near a + x
  Tree* lowerIntegrand = RewriteIntegrandNear(integrand, lowerBound);
  insertAt->moveTreeAtNode(lowerIntegrand);
  e->cloneNodeOverNode(KIntegralWithAlternatives);
  return true;
}

bool PrepareExpressionForApproximation(Tree* e) {
  assert(!Dependency::DeepBubbleUpDependencies(e));
  assert(!Dependency::DeepRemoveUselessDependencies(e));
  bool changed = Tree::ApplyShallowTopDown(e, &ShallowExpandIntegrals);
  if (changed) {
    /* ShallowExpandIntegrals can introduce dependencies. Some could be removed,
     * but no situation with a need for a bubble up has been found yet.
     * Call it if it becomes a need. */
    assert(!Dependency::DeepBubbleUpDependencies(e));
    Dependency::DeepRemoveUselessDependencies(e);
  }
  if (Tree::ApplyShallowTopDown(e, &ShallowPrepareForApproximation)) {
    changed = true;
    /* ShallowPrepareForApproximation is not expected to add further
     * dependencies to prevent exponential growth in expression size. */
    assert(!Dependency::DeepBubbleUpDependencies(e));
    /* After ShallowPrepareForApproximation, some nodes have been un-projected,
     * and ContainsSameDependency, called in DeepRemoveUselessDependencies,
     * doesn't handle it. It is therefore skipped. */
  }
  return changed;
}

}  // namespace Private

void PrepareFunctionForApproximation(Tree* e, const char* variable,
                                     bool variableIsReal) {
  assert(Simplification::IsSystem(e));
  Variables::ReplaceSymbol(e, variable, 0,
                           variableIsReal ? ComplexProperties::Real()
                                          : ComplexProperties::Unknown());
  /* All symbols should already have been replaced by their definition.
   * Leftover symbols can be replaced by undefined. */
  Projection::DeepReplaceUserNamed(
      e, EmptySymbolContext{},
      SymbolicComputation::ReplaceAllSymbolsWithUndefined);
  e->moveTreeOverTree(ToTree<double>(
      e,
      Parameters{
          .isRootAndCanHaveRandom = true, .prepare = true, .optimize = true}));
}

}  // namespace Poincare::Internal::Approximation
