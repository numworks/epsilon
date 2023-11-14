#include "projection.h"

#include <poincare_junior/src/expression/decimal.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/pattern_matching.h>

namespace PoincareJ {

bool Projection::DeepSystemProjection(Tree* ref,
                                      ProjectionContext projectionContext) {
  bool changed =
      (projectionContext.m_strategy == Strategy::ApproximateToFloat) &&
      Approximation::ApproximateAndReplaceEveryScalar(ref);
  return Tree::ApplyShallowInDepth(ref, ShallowSystemProjection,
                                   &projectionContext) ||
         changed;
}

/* The order of nodes in NAry is not a concern here. They will be sorted before
 * SystemReduction. */
bool Projection::ShallowSystemProjection(Tree* ref, void* context) {
  /* TODO: Most of the projections could be optimized by simply replacing and
   * inserting nodes. This optimization could be applied in matchAndReplace. See
   * comment in matchAndReplace. */
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);

  bool changed = false;
  if (ref->isUndefined()) {
    ExceptionCheckpoint::Raise(ExceptionType::Unhandled);
  }
  if (ref->isUnit()) {
    Units::Unit::RemoveUnit(ref);
    changed = true;
  }

  if (projectionContext->m_strategy == Strategy::NumbersToFloat &&
      ref->isNumber()) {
    return Approximation::ApproximateAndReplaceEveryScalar(ref) || changed;
  }

  if (ref->isDecimal()) {
    Decimal::Project(ref);
    return true;
  }

  // Project angles depending on context
  PoincareJ::AngleUnit angleUnit = projectionContext->m_angleUnit;
  if (ref->isOfType(
          {BlockType::ArcSine, BlockType::ArcCosine, BlockType::ArcTangent}) &&
      angleUnit != PoincareJ::AngleUnit::Radian) {
    // TODO: Handle inverse trigonometric angle conversions.
    return changed;
  }
  if (ref->isOfType({BlockType::Sine, BlockType::Cosine, BlockType::Tangent}) &&
      angleUnit != PoincareJ::AngleUnit::Radian) {
    Tree* child = ref->child(0);
    child->moveTreeOverTree(PatternMatching::Create(
        KMult(KA, π_e, KPow(KB, -1_e)),
        {.KA = child,
         .KB = (angleUnit == PoincareJ::AngleUnit::Degree ? 180_e : 200_e)}));
  }

  // inf -> Float(inf) to prevent inf-inf from being 0
  if (ref->isInfinity()) {
    /* TODO: Raise to try projecting again with ApproximateToFloat strategy.
     *       Do not handle float contamination unless this is the strategy.
     *       Later, handle exact inf (∞-∞, ∞^0, 0+, 0-, ...) and remove Raise.*/
    projectionContext->m_strategy = Strategy::ApproximateToFloat;
    // ∞ -> Float(∞)
    return Approximation::ApproximateAndReplaceEveryScalar(ref) || changed;
  }

  // Sqrt(A) -> A^0.5
  changed = PatternMatching::MatchAndReplace(ref, KSqrt(KA), KPow(KA, KHalf));
  if (ref->isPower()) {
    if (PatternMatching::MatchAndReplace(ref, KPow(e_e, KA), KExp(KA))) {
    } else if (Dimension::GetDimension(ref->nextNode()).isMatrix()) {
      ref->cloneNodeOverNode(KPowMatrix);
    } else if (projectionContext->m_complexFormat == ComplexFormat::Real) {
      ref->cloneNodeOverNode(KPowReal);
    } else {
      return changed;
    }
    return true;
  }

  /* In following replacements, ref node isn't supposed to be replaced with a
   * node needing further projection. */
  return
      // e -> exp(1)
      PatternMatching::MatchAndReplace(ref, e_e, KExp(1_e)) ||
      // conj(A) -> re(A)-i*re(A)
      PatternMatching::MatchAndReplace(
          ref, KConj(KA), KComplex(KRe(KA), KMult(-1_e, KIm(KA)))) ||
      // i -> Complex(0,1)
      PatternMatching::MatchAndReplace(ref, i_e, KComplex(0_e, 1_e)) ||
      // - A  -> (-1)*A
      PatternMatching::MatchAndReplace(ref, KOpposite(KA), KMult(-1_e, KA)) ||
      // A - B -> A + (-1)*B
      PatternMatching::MatchAndReplace(ref, KSub(KA, KB),
                                       KAdd(KA, KMult(-1_e, KB))) ||
      // A / B -> A * B^-1
      PatternMatching::MatchAndReplace(ref, KDiv(KA, KB),
                                       KMult(KA, KPow(KB, -1_e))) ||
      // cos(A) -> trig(A, 0)
      PatternMatching::MatchAndReplace(ref, KCos(KA), KTrig(KA, 0_e)) ||
      // sin(A) -> trig(A, 1)
      PatternMatching::MatchAndReplace(ref, KSin(KA), KTrig(KA, 1_e)) ||
      // tan(A) -> sin(A) * cos(A)^(-1)
      /* TODO: Tangent will duplicate its yet to be projected children,
       * replacing it after everything else may be an optimization.
       * Sin and cos terms will be replaced afterwards. */
      PatternMatching::MatchAndReplace(ref, KTan(KA),
                                       KMult(KSin(KA), KPow(KCos(KA), -1_e))) ||
      // acos(A) -> atrig(A, 0)
      PatternMatching::MatchAndReplace(ref, KACos(KA), KATrig(KA, 0_e)) ||
      // asin(A) -> atrig(A, 1)
      PatternMatching::MatchAndReplace(ref, KASin(KA), KATrig(KA, 1_e)) ||
      // atan(A) -> asin(A/Sqrt(1+a^2)) - TODO: Do this in AdvancedReduction
      PatternMatching::MatchAndReplace(
          ref, KATan(KA),
          KATrig(KDiv(KA, KSqrt(KAdd(1_e, KPow(KA, 2_e)))), 1_e)) ||
      // log(A, e) -> ln(e)
      PatternMatching::MatchAndReplace(ref, KLogarithm(KA, e_e), KLn(KA)) ||
      // log(A) -> ln(A) * ln(10)^(-1)
      // TODO: Maybe log(A) -> log(A, 10) and rely on next matchAndReplace
      PatternMatching::MatchAndReplace(ref, KLog(KA),
                                       KMult(KLn(KA), KPow(KLn(10_e), -1_e))) ||
      // log(A, B) -> ln(A) * ln(B)^(-1)
      PatternMatching::MatchAndReplace(ref, KLogarithm(KA, KB),
                                       KMult(KLn(KA), KPow(KLn(KB), -1_e))) ||
      changed;
}

}  // namespace PoincareJ
