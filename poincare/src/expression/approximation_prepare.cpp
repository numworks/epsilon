#include <poincare/src/memory/pattern_matching.h>

#include "approximation.h"
#include "beautification.h"
#include "k_tree.h"
#include "simplification.h"
#include "variables.h"

namespace Poincare::Internal {

bool Approximation::ShallowPrepareForApproximation(Tree* expr, void* ctx) {
  // TODO: we want x^-1 -> 1/x and y*x^-1 -> y/x but maybe not x^-2 -> 1/x^2 ?
  return PatternMatching::MatchReplace(expr, KPow(KA, -1_e), KDiv(1_e, KA)) ||
         PatternMatching::MatchReplace(expr, KExp(KMult(KA_s, KLn(KB), KC_s)),
                                       KPow(KB, KMult(KA_s, KC_s))) ||
         PatternMatching::MatchReplace(expr, KPowReal(KA, 1_e / 2_e),
                                       KSqrt(KA)) ||
         PatternMatching::MatchReplace(expr, KPow(KA, 1_e / 2_e), KSqrt(KA));
}

void Approximation::PrepareFunctionForApproximation(
    Tree* expr, const char* variable, AngleUnit angleUnit,
    ComplexFormat complexFormat) {
  ProjectionContext projectionContext = {
      .m_complexFormat = complexFormat,
      .m_angleUnit = angleUnit,
  };
  // TODO_PCJ: Factorize this with Simplification::Simplify
  Simplification::PrepareForProjection(expr, projectionContext);
  assert(!Simplification::ExtractUnits(expr, &projectionContext));
  Variables::ReplaceSymbol(expr, variable, 0, ComplexSign::RealUnknown());
  Projection::DeepSystemProject(expr, projectionContext);
  // TODO: Simplification with NumbersToScalar if preparing function
  Tree::ApplyShallowInDepth(expr, &ShallowPrepareForApproximation);
  // TODO: factor common sub-expressions
  // TODO: apply Horner's method: a*x^2 + b*x + c => (a*x + b)*x + c ?
}

}  // namespace Poincare::Internal
