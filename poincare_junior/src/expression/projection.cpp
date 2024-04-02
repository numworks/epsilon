#include "projection.h"

#include <apps/global_preferences.h>
#include <poincare/preferences.h>
#include <poincare_junior/src/expression/angle.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/decimal.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/pattern_matching.h>

namespace PoincareJ {

ProjectionContext Projection::ContextFromSettings() {
  return ProjectionContext{
      .m_complexFormat =
          Poincare::Preferences::SharedPreferences()->complexFormat(),
      .m_angleUnit = Poincare::Preferences::SharedPreferences()->angleUnit(),
      .m_strategy = Strategy::Default,
      .m_dimension = Dimension(),
      .m_unitFormat =
          GlobalPreferences::SharedGlobalPreferences()->unitFormat(),
  };
}

bool Projection::DeepSystemProject(Tree* ref,
                                   ProjectionContext projectionContext) {
  bool changed = false;
  if (projectionContext.m_strategy != Strategy::Default) {
    assert((projectionContext.m_strategy == Strategy::ApproximateToFloat ||
            projectionContext.m_strategy == Strategy::NumbersToFloat));
    changed = Approximation::ApproximateAndReplaceEveryScalar(
        ref, projectionContext.m_strategy == Strategy::ApproximateToFloat,
        &projectionContext);
  }
  return Tree::ApplyShallowInDepth(ref, ShallowSystemProject,
                                   &projectionContext) ||
         changed;
}

/* The order of nodes in NAry is not a concern here. They will be sorted before
 * SystemReduction. */
bool Projection::ShallowSystemProject(Tree* ref, void* context) {
  /* TODO: Most of the projections could be optimized by simply replacing and
   * inserting nodes. This optimization could be applied in matchAndReplace. See
   * comment in matchAndReplace. */
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);

  bool changed = false;
  if (ref->isUndefined()) {
    ExceptionCheckpoint::Raise(ExceptionType::Unhandled);
  }
  if (ref->isParenthesis()) {
    ref->removeNode();
    ShallowSystemProject(ref, context);
    return true;
  }
  if (ref->isUnit()) {
    Units::Unit::RemoveUnit(ref);
    changed = true;
  }
  if (ref->isPhysicalConstant()) {
    Tree* value = SharedEditionPool->push<BlockType::DoubleFloat>(
        Constant::Info(ref).m_value);
    ref->moveTreeOverTree(value);
    return true;
  }

  if (ref->isDecimal()) {
    Decimal::Project(ref);
    changed = true;
  }

  // Project angles depending on context
  PoincareJ::AngleUnit angleUnit = projectionContext->m_angleUnit;
  if (ref->isOfType({BlockType::Sine, BlockType::Cosine, BlockType::Tangent}) &&
      angleUnit != PoincareJ::AngleUnit::Radian) {
    Tree* child = ref->child(0);
    child->moveTreeOverTree(PatternMatching::Create(
        KMult(KA, KB), {.KA = child, .KB = Angle::ToRad(angleUnit)}));
    changed = true;
  } else if (ref->isOfType({BlockType::ArcSine, BlockType::ArcCosine,
                            BlockType::ArcTangent})) {
    /* Project inverse trigonometric functions here to avoid infinite projection
     * to radian loop. */
    // acos(A) -> atrig(A, 0)
    PatternMatching::MatchAndReplace(ref, KACos(KA), KATrig(KA, 0_e)) ||
        // asin(A) -> atrig(A, 1)
        PatternMatching::MatchAndReplace(ref, KASin(KA), KATrig(KA, 1_e)) ||
        // atan(A) -> atanRad(A)
        PatternMatching::MatchAndReplace(ref, KATan(KA), KATanRad(KA));
    if (angleUnit != PoincareJ::AngleUnit::Radian) {
      // arccos_degree(x) = arccos_radians(x) * 180/π
      ref->moveTreeOverTree(PatternMatching::Create(
          KMult(KA, KB), {.KA = ref, .KB = Angle::RadTo(angleUnit)}));
    }
    return true;
  }

  // inf -> Float(inf) to prevent inf-inf from being 0
  if (ref->isInfinity()) {
    /* TODO: Infinity is only handled as float. Raise to try again with float
     * numbers, preventing from having to handle float contamination.
     * Later, handle exact inf (∞-∞, ∞^0, 0+, 0-, ...) and remove this Raise.*/
    ExceptionCheckpoint::Raise(ExceptionType::RelaxContext);
  }

  // Under Real complex format, use node alternative to properly handle nonreal.
  bool realMode = projectionContext->m_complexFormat == ComplexFormat::Real;
  if (ref->isPower()) {
    if (PatternMatching::MatchAndReplace(ref, KPow(e_e, KA), KExp(KA))) {
    } else if (Dimension::GetDimension(ref->nextNode()).isMatrix()) {
      ref->cloneNodeOverNode(KPowMatrix);
    } else if (realMode) {
      ref->cloneNodeOverNode(KPowReal);
    } else {
      return changed;
    }
    return true;
  }

  if (realMode && ref->isLn()) {
    ref->cloneNodeOverNode(KLnReal);
    return true;
  }

  if (  // Sqrt(A) -> A^0.5
      PatternMatching::MatchAndReplace(ref, KSqrt(KA), KPow(KA, KHalf)) ||
      // NthRoot(A, B) -> A^(1/B)
      PatternMatching::MatchAndReplace(ref, KNthRoot(KA, KB),
                                       KPow(KA, KPow(KB, -1_e))) ||
      // log(A, e) -> ln(e)
      PatternMatching::MatchAndReplace(ref, KLogarithm(KA, e_e), KLn(KA)) ||
      // Sec(A) -> 1/cos(A)
      PatternMatching::MatchAndReplace(ref, KSec(KA), KPow(KCos(KA), -1_e)) ||
      // Csc(A) -> 1/sin(A)
      PatternMatching::MatchAndReplace(ref, KCsc(KA), KPow(KSin(KA), -1_e)) ||
      // ArcSec(A) -> acos(1/A)
      PatternMatching::MatchAndReplace(ref, KArcSec(KA),
                                       KACos(KPow(KA, -1_e))) ||
      // ArcCsc(A) -> asin(1/A)
      PatternMatching::MatchAndReplace(ref, KArcCsc(KA),
                                       KASin(KPow(KA, -1_e))) ||
      // ArCosh(A) -> ln(A+sqrt(A-1)*sqrt(A+1))
      PatternMatching::MatchAndReplace(
          ref, KArCosh(KA),
          KLn(KAdd(KA, KMult(KSqrt(KAdd(KA, -1_e)), KSqrt(KAdd(KA, 1_e)))))) ||
      // ArSinh(A) -> ln(A+sqrt(A^2+1))
      PatternMatching::MatchAndReplace(
          ref, KArSinh(KA), KLn(KAdd(KA, KSqrt(KAdd(KPow(KA, 2_e), 1_e)))))) {
    // Ref node may need to be projected again.
    ShallowSystemProject(ref, context);
    return true;
  }

  /* In following replacements, ref node isn't supposed to be replaced with
   * a node needing further projection. */
  return
      // ceil(A)  -> -floor(-A)
      PatternMatching::MatchAndReplace(ref, KCeil(KA),
                                       KMult(-1_e, KFloor(KMult(-1_e, KA)))) ||
      // frac(A) -> A - floor(A)
      PatternMatching::MatchAndReplace(ref, KFrac(KA),
                                       KAdd(KA, KMult(-1_e, KFloor(KA)))) ||
      // e -> exp(1)
      PatternMatching::MatchAndReplace(ref, e_e, KExp(1_e)) ||
      // conj(A) -> re(A)-i*re(A)
      PatternMatching::MatchAndReplace(
          ref, KConj(KA), KAdd(KRe(KA), KMult(-1_e, i_e, KIm(KA)))) ||
      // - A  -> (-1)*A
      PatternMatching::MatchAndReplace(ref, KOpposite(KA), KMult(-1_e, KA)) ||
      // A - B -> A + (-1)*B
      PatternMatching::MatchAndReplace(ref, KSub(KA, KB),
                                       KAdd(KA, KMult(-1_e, KB))) ||
      // A / B -> A * B^-1
      PatternMatching::MatchAndReplace(ref, KDiv(KA, KB),
                                       KMult(KA, KPow(KB, -1_e))) ||
      // MixedFraction(A + B/C) -> A + B/C
      // TODO assert KB is a simple rational
      PatternMatching::MatchAndReplace(ref, KMixedFraction(KA, KB),
                                       KAdd(KA, KB)) ||
      // cos(A) -> trig(A, 0)
      PatternMatching::MatchAndReplace(ref, KCos(KA), KTrig(KA, 0_e)) ||
      // sin(A) -> trig(A, 1)
      PatternMatching::MatchAndReplace(ref, KSin(KA), KTrig(KA, 1_e)) ||
      // tan(A) -> tanRad(A, 1)
      PatternMatching::MatchAndReplace(ref, KTan(KA), KTanRad(KA)) ||
      // log(A) -> ln(A) * ln(10)^(-1)
      // TODO: Maybe log(A) -> log(A, 10) and rely on next matchAndReplace
      PatternMatching::MatchAndReplace(ref, KLog(KA),
                                       KMult(KLn(KA), KPow(KLn(10_e), -1_e))) ||
      // log(A, B) -> ln(A) * ln(B)^(-1)
      PatternMatching::MatchAndReplace(ref, KLogarithm(KA, KB),
                                       KMult(KLn(KA), KPow(KLn(KB), -1_e))) ||
      // Cot(A) -> cos(A)/sin(A) (Avoid tan to for dependencies)
      PatternMatching::MatchAndReplace(ref, KCot(KA),
                                       KMult(KCos(KA), KPow(KSin(KA), -1_e))) ||
      /* ArcCot(A) -> { π/2 if A is 0, atan(1/A) otherwise } using acos(0)
       * instead of π/2 to handle angle unit */
      PatternMatching::MatchAndReplace(
          ref, KArcCot(KA),
          KPiecewise(KACos(0_e), KEqual(KA, 0_e), KATan(KPow(KA, -1_e)))) ||
      // Cosh(A) -> (exp(A)+exp(-A))*1/2
      PatternMatching::MatchAndReplace(
          ref, KCosh(KA),
          KMult(KHalf, KAdd(KExp(KA), KExp(KMult(-1_e, KA))))) ||
      // Sinh(A) -> (exp(A)-exp(-A))*1/2
      PatternMatching::MatchAndReplace(
          ref, KSinh(KA),
          KMult(KHalf, KAdd(KExp(KA), KMult(-1_e, KExp(KMult(-1_e, KA)))))) ||
      // Tanh(A) -> (exp(2A)-1)/(exp(2A)+1)
      PatternMatching::MatchAndReplace(
          ref, KTanh(KA),
          KMult(KAdd(KExp(KMult(2_e, KA)), -1_e),
                KPow(KAdd(KExp(KMult(2_e, KA)), 1_e), -1_e))) ||
      // ArTanh(A) -> (ln(1+A)-ln(1-A))*1/2
      PatternMatching::MatchAndReplace(
          ref, KArTanh(KA),
          KMult(KHalf, KAdd(KLn(KAdd(1_e, KA)),
                            KMult(-1_e, KLn(KAdd(1_e, KMult(-1_e, KA))))))) ||
      // A nor B -> not (A or B)
      PatternMatching::MatchAndReplace(ref, KLogicalNor(KA, KB),
                                       KLogicalNot(KLogicalOr(KA, KB))) ||
      // A nand B -> not (A and B)
      PatternMatching::MatchAndReplace(ref, KLogicalNand(KA, KB),
                                       KLogicalNot(KLogicalAnd(KA, KB))) ||
      changed;
}

bool Projection::Expand(Tree* tree) {
  return
      // tan(A) -> sin(A) * cos(A)^(-1)
      PatternMatching::MatchReplaceAndSimplify(
          tree, KTanRad(KA),
          KMult(KTrig(KA, 1_e), KPow(KTrig(KA, 0_e), -1_e))) ||
      // TODO: This expansion introduces KPow when KPowReal could be needed.
      // atan(A) -> asin(A/Sqrt(1 + A^2))
      PatternMatching::MatchReplaceAndSimplify(
          tree, KATanRad(KA),
          KATrig(KMult(KA, KPow(KAdd(1_e, KPow(KA, 2_e)), KMult(-1_e, KHalf))),
                 1_e));
}

}  // namespace PoincareJ
