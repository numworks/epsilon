#include <poincare/src/memory/pattern_matching.h>

#include "advanced_reduction.h"
#include "approximation.h"
#include "beautification.h"
#include "dependency.h"
#include "k_tree.h"
#include "systematic_reduction.h"
#include "variables.h"

namespace Poincare::Internal {

bool Approximation::ShallowPrepareForApproximation(Tree* e, void* ctx) {
  // TODO: we want x^-1 -> 1/x and y*x^-1 -> y/x but maybe not x^-2 -> 1/x^2 ?
  bool changed = PatternMatching::MatchReplace(
      e, KExp(KMult(KA_s, KLn(KB), KC_s)), KPow(KB, KMult(KA_s, KC_s)));
  return PatternMatching::MatchReplace(e, KPowReal(KA, -1_e), KDiv(1_e, KA)) ||
         PatternMatching::MatchReplace(e, KPow(KA, -1_e), KDiv(1_e, KA)) ||
         PatternMatching::MatchReplace(
             e, KPowReal(KA, 1_e / 2_e),
             KDep(KSqrt(KA), KDepList(KRealPos(KA)))) ||
         PatternMatching::MatchReplace(e, KPow(KA, 1_e / 2_e), KSqrt(KA)) ||
         /* TODO: e^ is better than exp because we have code to handle special
          * cases in pow, exp is probably more precise on normal values */
         PatternMatching::MatchReplace(e, KExp(KA), KPow(e_e, KA)) || changed;
}

Tree* RewriteIntegrandNear(const Tree* integrand, const Tree* bound) {
  Tree* value = SharedTreeStack->pushAdd(2);
  bound->cloneTree();
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

bool ShallowExpandIntegrals(Tree* e, void* ctx) {
  if (!e->isIntegral()) {
    return false;
  }
  Tree* insertAt = e->nextTree();
  // Rewrite the integrand to be able to compute it directly at abscissa b - x
  Tree* upperIntegrand = RewriteIntegrandNear(e->child(3), e->child(2));
  insertAt->moveTreeBeforeNode(upperIntegrand);
  // Same near a + x
  Tree* lowerIntegrand = RewriteIntegrandNear(e->child(3), e->child(1));
  insertAt->moveTreeBeforeNode(lowerIntegrand);
  e->cloneNodeOverNode(KIntegralWithAlternatives);
  return true;
}

bool Approximation::PrepareFunctionForApproximation(
    Tree* e, const char* variable, ComplexFormat complexFormat) {
  Variables::ReplaceSymbol(e, variable, 0,
                           complexFormat == ComplexFormat::Real
                               ? ComplexSign::RealUnknown()
                               : ComplexSign::Unknown());
  e->moveTreeOverTree(ToTree<double>(e, Parameter(true, false, true, true),
                                     Context(AngleUnit::None, complexFormat)));
  return true;
}

bool Approximation::PrepareExpressionForApproximation(
    Tree* e, ComplexFormat complexFormat) {
  bool changed = Tree::ApplyShallowTopDown(e, &ShallowExpandIntegrals);
  changed =
      Tree::ApplyShallowTopDown(e, &ShallowPrepareForApproximation) || changed;
  if (changed) {
    // ShallowPrepareForApproximation can introduce dependencies
    Dependency::DeepBubbleUpDependencies(e);
  }
  return changed;
}

}  // namespace Poincare::Internal
