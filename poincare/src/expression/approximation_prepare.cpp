#include <poincare/src/memory/pattern_matching.h>

#include "advanced_simplification.h"
#include "approximation.h"
#include "beautification.h"
#include "k_tree.h"
#include "simplification.h"
#include "variables.h"

namespace Poincare::Internal {

bool Approximation::ShallowPrepareForApproximation(Tree* expr, void* ctx) {
  // TODO: we want x^-1 -> 1/x and y*x^-1 -> y/x but maybe not x^-2 -> 1/x^2 ?
  bool changed = PatternMatching::MatchReplace(
      expr, KExp(KMult(KA_s, KLn(KB), KC_s)), KPow(KB, KMult(KA_s, KC_s)));
  return PatternMatching::MatchReplace(expr, KPowReal(KA, 1_e / 2_e),
                                       KSqrt(KA)) ||
         PatternMatching::MatchReplace(expr, KPow(KA, -1_e), KDiv(1_e, KA)) ||
         PatternMatching::MatchReplace(expr, KPow(KA, 1_e / 2_e), KSqrt(KA)) ||
         /* TODO: e^ is better than exp because we have code to handle special
          * cases in pow, exp is probably more precise on normal values */
         PatternMatching::MatchReplace(expr, KExp(KA), KPow(e_e, KA)) ||
         changed;
}

Tree* RewriteIntegrandNear(const Tree* integrand, const Tree* bound) {
  Tree* value = SharedTreeStack->pushAdd(2);
  bound->cloneTree();
  KVarX->cloneTree();
  Simplification::DeepSystematicReduce(value);
  Tree* tree = integrand->cloneTree();
  // TODO: This deep should have been done before
  Simplification::DeepSystematicReduce(tree);
  Variables::Replace(tree, 0, value, false, true);
  /* We need to remove the constant part by expanding polynomials introduced by
   * the replacement, e.g. 1-(1-x)^2 -> 2x-x^2 */
  AdvancedSimplification::DeepExpand(tree);
  value->removeTree();
  return value;
}

bool ShallowExpandIntegrals(Tree* expr, void* ctx) {
  if (!expr->isIntegral()) {
    return false;
  }
  Tree* insertAt = expr->nextTree();
  // Rewrite the integrand to be able to compute it directly at abscissa b - x
  Tree* upperIntegrand = RewriteIntegrandNear(expr->child(3), expr->child(2));
  insertAt->moveTreeBeforeNode(upperIntegrand);
  // Same near a + x
  Tree* lowerIntegrand = RewriteIntegrandNear(expr->child(3), expr->child(1));
  insertAt->moveTreeBeforeNode(lowerIntegrand);
  expr->cloneNodeOverNode(KIntegralWithAlternatives);
  return true;
}

bool Approximation::PrepareFunctionForApproximation(
    Tree* expr, const char* variable, ComplexFormat complexFormat) {
  bool changed = Variables::ReplaceSymbol(expr, variable, 0,
                                          complexFormat == ComplexFormat::Real
                                              ? ComplexSign::RealUnknown()
                                              : ComplexSign::Unknown());
  changed = PrepareExpressionForApproximation(expr, complexFormat);
  changed = ApproximateAndReplaceEveryScalar(expr) || changed;
  // TODO: factor common sub-expressions
  // TODO: apply Horner's method: a*x^2 + b*x + c => (a*x + b)*x + c ?
  return changed;
}

bool Approximation::PrepareExpressionForApproximation(
    Tree* expr, ComplexFormat complexFormat) {
  bool changed = Tree::ApplyShallowInDepth(expr, &ShallowExpandIntegrals);
  changed = Tree::ApplyShallowInDepth(expr, &ShallowPrepareForApproximation) ||
            changed;
  return changed;
}

}  // namespace Poincare::Internal
