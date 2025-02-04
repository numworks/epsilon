#include "projection.h"

#include <poincare/preferences.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree_helpers.h>

#include "angle.h"
#include "decimal.h"
#include "dependency.h"
#include "physical_constant.h"
#include "sign.h"
#include "symbol.h"
#include "units/representatives.h"
#include "units/unit.h"
#include "variables.h"

namespace Poincare::Internal {

bool Projection::DeepReplaceUserNamed(Tree* e, Poincare::Context* context,
                                      SymbolicComputation symbolic) {
  /* TODO_PCJ: in old poincare, we did not do anything for sequence trees (not
   * only nodes), and for store trees we only replaced in the first child and if
   * the second child was a user symbol. */
  if (symbolic == SymbolicComputation::KeepAllSymbols) {
    return false;
  }
  // Check for circularity
  if (Symbol::InvolvesCircularity(e, context)) {
    e->cloneTreeOverTree(KUndef);
    return true;
  }
  bool changed = false;
  /* ShallowReplaceUserNamed may push and remove trees at the end of TreeStack.
   * We use a marker to keep track of the initial end position of e. */
  TreeRef marker = pushEndMarker(e);
  while (e->block() < marker->block()) {
    if (e->isParametric()) {
      // Skip Parametric node and its variable, never replaced.
      static_assert(Parametric::k_variableIndex == 0);
      e = e->nextNode()->nextTree();
    }
    changed = ShallowReplaceUserNamed(e, context, symbolic) || changed;
    e = e->nextNode();
  }
  removeMarker(marker);
  return changed;
}

bool Projection::ShallowReplaceUserNamed(Tree* e, Poincare::Context* context,
                                         SymbolicComputation symbolic) {
  assert(symbolic != SymbolicComputation::KeepAllSymbols);
  bool eIsUserFunction = e->isUserFunction();
  if (!eIsUserFunction &&
      (!e->isUserSymbol() ||
       symbolic == SymbolicComputation::ReplaceDefinedFunctions)) {
    // TODO: skip also system symbol?
    return false;
  }
  if (symbolic == SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    e->cloneTreeOverTree(KNotDefined);
    return true;
  }
  // Get Definition
  const Tree* definition =
      context ? context->expressionForUserNamed(e) : nullptr;
  if (symbolic == SymbolicComputation::ReplaceAllSymbols && !definition) {
    e->cloneTreeOverTree(KNotDefined);
    return true;
  } else if (!definition) {
    return false;
  }
  if (eIsUserFunction) {
    // Replace function's symbol with definition
    Variables::ReplaceUserFunctionOrSequenceWithTree(e, definition);
  } else {
    // Otherwise, local variable scope should be handled.
    assert(!Variables::HasVariables(definition));
    e->cloneTreeOverTree(definition);
  }
  // Replace node again in case it has been replaced with another symbol
  ShallowReplaceUserNamed(e, context, symbolic);
  return true;
}

bool hasComplexNodes(const Tree* e, ProjectionContext& projectionContext) {
  // If it lives in the pool, fetching definitions may invalidate it.
  bool mayReplaceSymbols =
      (projectionContext.m_symbolic != SymbolicComputation::KeepAllSymbols &&
       projectionContext.m_symbolic !=
           SymbolicComputation::ReplaceAllSymbolsWithUndefined);
  if (mayReplaceSymbols && !SharedTreeStack->contains(e)) {
    /* Clone tree in treeStack to prevent address being invalidated while
     * fetching symbols definitions in the pool. TODO_PCJ: Fix this in
     * ExpressionForSymbolAndRecord method. */
    Tree* treeOnStack = e->cloneTree();
    bool result = hasComplexNodes(treeOnStack, projectionContext);
    treeOnStack->removeTree();
    return result;
  }
  for (const Tree* descendant : e->selfAndDescendants()) {
    if (descendant->isOfType(
            {Type::ComplexI, Type::Conj, Type::Im, Type::Re, Type::Arg})) {
      return true;
    }
    // Skip UserSequences.
    if (descendant->isUserFunction() || descendant->isUserSymbol()) {
      /* We could factorize with DeepReplaceUserNamed but this avoid having to
       * clone the tree. */
      switch (projectionContext.m_symbolic) {
        case SymbolicComputation::KeepAllSymbols:
        case SymbolicComputation::ReplaceAllSymbolsWithUndefined:
          break;
        case SymbolicComputation::ReplaceDefinedFunctions:
          if (!e->isUserFunction()) {
            break;
          }
          [[fallthrough]];
        default: {
          const Tree* definition =
              projectionContext.m_context
                  ? projectionContext.m_context->expressionForUserNamed(
                        descendant)
                  : nullptr;
          assert(definition != e);
          if (definition && hasComplexNodes(definition, projectionContext)) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool Projection::UpdateComplexFormatWithExpressionInput(
    const Tree* e, ProjectionContext* projectionContext) {
  assert(projectionContext);
  if (e && projectionContext->m_complexFormat == ComplexFormat::Real &&
      !Symbol::InvolvesCircularity(e, projectionContext->m_context) &&
      hasComplexNodes(e, *projectionContext)) {
    projectionContext->m_complexFormat =
        Preferences::k_defaultComplexFormatIfNotReal;
    return true;
  }
  return false;
}

bool Projection::DeepSystemProject(Tree* e,
                                   ProjectionContext projectionContext) {
  bool changed = false;
  if (e->isEuclideanDivision()) {
    /* The euclidian division node is kept only at the root level and
     * it replaced by quo in ShallowSystemProject otherwise. */
    changed |= Tree::ApplyShallowTopDown(e->child(0), ShallowSystemProject,
                                         &projectionContext);
    changed |= Tree::ApplyShallowTopDown(e->child(1), ShallowSystemProject,
                                         &projectionContext);
  } else {
    changed |=
        Tree::ApplyShallowTopDown(e, ShallowSystemProject, &projectionContext);
  }
  assert(!e->hasDescendantSatisfying(Projection::IsForbidden));
  if (changed) {
    Dependency::DeepBubbleUpDependencies(e);
    Dependency::DeepRemoveUselessDependencies(e);
  }
  return changed;
}

bool Projection::IsForbidden(const Tree* e) {
  Poincare::ExamMode examMode =
      Poincare::Preferences::SharedPreferences()->examMode();
  switch (e->type()) {
    case Type::PhysicalConstant:
    case Type::Unit:
      return examMode.forbidUnits();
    case Type::LogBase:
      return examMode.forbidBasedLogarithm();
    case Type::Sum:
      return examMode.forbidSum();
    case Type::Norm:
      return examMode.forbidVectorNorm();
    case Type::Cross:
    case Type::Dot:
      return examMode.forbidVectorProduct();
    default:
      return false;
  }
}

/* The order of nodes in NAry is not a concern here. They will be sorted before
 * SystemReduction. */
bool Projection::ShallowSystemProject(Tree* e, void* context) {
  /* TODO: Most of the projections could be optimized by simply replacing
   * and inserting nodes. This optimization could be applied in
   * matchAndReplace. See comment in matchAndReplace. */
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);

  if (IsForbidden(e)) {
    e->cloneTreeOverTree(KForbidden);
    return true;
  }
  if (e->isParentheses()) {
    e->removeNode();
    ShallowSystemProject(e, context);
    return true;
  }
  bool changed = false;
  if (e->isDecimal()) {
    Decimal::Project(e);
    changed = true;
  }
  if (e->isUnitOrPhysicalConstant() &&
      projectionContext->m_dimension.hasNonKelvinTemperatureUnit() &&
      Units::Unit::GetRepresentative(e)->siVector() !=
          Units::Temperature::Dimension) {
    /* To prevent unnecessary mix of units
     * (12_km / 6_mm)×_°C -> (12 000 / 0.006)×_°C */
    Units::Unit::RemoveUnit(e);
  }

  // Project angles depending on context
  AngleUnit angleUnit = projectionContext->m_angleUnit;
  if (e->isOfType({Type::Sin, Type::Cos, Type::Tan})) {
    /* In degree, cos(23°) and cos(23) -> trig(23×π/180, 0)
     * but        cos(23rad)           -> trig(23      , 0) */
    Tree* child = e->child(0);
    Dimension childDim = Dimension::Get(child);
    if (childDim.isSimpleAngleUnit()) {
      // Remove all units to fall back to radian
      changed = Tree::ApplyShallowTopDown(e, Units::Unit::ShallowRemoveUnit);
    } else if (angleUnit != AngleUnit::Radian) {
      child->moveTreeOverTree(PatternMatching::Create(
          KMult(KA, KB), {.KA = child, .KB = Angle::ToRad(angleUnit)}));
      changed = true;
    }
  } else if (e->isOfType({Type::ASin, Type::ACos, Type::ATan})) {
    /* Project inverse trigonometric functions here to avoid infinite projection
     * to radian loop. */
    if (projectionContext->m_complexFormat == ComplexFormat::Real &&
        !e->isATan()) {
#if POINCARE_PIECEWISE
      // Only real functions asin and acos have a domain of definition
      // acos(A) -> atrig(A, 0) if -1 <= A <= 1
      PatternMatching::MatchReplace(
          e, KACos(KA),
          KDep(KATrig(KA, 0_e),
               KDepList(KPiecewise(0_e, KInferiorEqual(KAbs(KA), 1_e),
                                   KNonReal)))) ||
          // asin(A) -> atrig(A, 1) if -1 <= A <= 1
          PatternMatching::MatchReplace(
              e, KASin(KA),
              KDep(KATrig(KA, 1_e),
                   KDepList(KPiecewise(0_e, KInferiorEqual(KAbs(KA), 1_e),
                                       KNonReal))));
#else
      // TODO fixme
      // acos(A) -> atrig(A, 0) if -1 <= A <= 1
      PatternMatching::MatchReplace(e, KACos(KA), KATrig(KA, 0_e)) ||
          // asin(A) -> atrig(A, 1) if -1 <= A <= 1
          PatternMatching::MatchReplace(e, KASin(KA), KATrig(KA, 1_e));
#endif
    } else {
      // acos(A) -> atrig(A, 0)
      PatternMatching::MatchReplace(e, KACos(KA), KATrig(KA, 0_e)) ||
          // asin(A) -> atrig(A, 1)
          PatternMatching::MatchReplace(e, KASin(KA), KATrig(KA, 1_e)) ||
          // atan(A) -> atanRad(A)
          PatternMatching::MatchReplace(e, KATan(KA), KATanRad(KA));
    }
    if (angleUnit != AngleUnit::Radian) {
      // arccos_degree(x) = arccos_radians(x) * 180/π
      e->moveTreeOverTree(PatternMatching::Create(
          KMult(KA, KB), {.KA = e, .KB = Angle::RadTo(angleUnit)}));
    }
    return true;
  }

  // Under Real complex format, use node alternative to properly handle nonreal.
  bool realMode = projectionContext->m_complexFormat == ComplexFormat::Real;
  if (e->isPow()) {
    if (PatternMatching::MatchReplace(e, KPow(e_e, KA), KExp(KA))) {
    } else
#if POINCARE_MATRIX
        if (Dimension::Get(e->child(0)).isMatrix()) {
      e->cloneNodeOverNode(KPowMatrix);
    } else
#endif
        if (realMode) {
      e->cloneNodeOverNode(KPowReal);
    } else {
      return changed;
    }
    return true;
  }

  if (e->isLnUser()) {
    if (realMode) {
      // lnUser(A) -> dep(ln(A), {nonNull(x), realPositive(x)})
      PatternMatching::MatchReplace(
          e, KLnUser(KA), KDep(KLn(KA), KDepList(KNonNull(KA), KRealPos(KA))));
    } else {
      // lnUser(A) -> dep(ln(A), {nonNull(x)})
      PatternMatching::MatchReplace(e, KLnUser(KA),
                                    KDep(KLn(KA), KDepList(KNonNull(KA))));
    }
    return true;
  }

  if (
      // Sqrt(A) -> A^0.5
      PatternMatching::MatchReplace(e, KSqrt(KA), KPow(KA, 1_e / 2_e)) ||
      // NthRoot(A, B) -> A^(1/B)
      PatternMatching::MatchReplace(e, KRoot(KA, KB),
                                    KPow(KA, KPow(KB, -1_e))) ||
      // log(A, e) -> ln(A)
      PatternMatching::MatchReplace(e, KLogBase(KA, e_e), KLnUser(KA)) ||
#if POINCARE_TRIGONOMETRY_ADVANCED
      // Cot(A) -> cos(A)/sin(A)
      PatternMatching::MatchReplace(e, KCot(KA),
                                    KMult(KCos(KA), KPow(KSin(KA), -1_e))) ||
      // Sec(A) -> 1/cos(A)
      PatternMatching::MatchReplace(e, KSec(KA), KPow(KCos(KA), -1_e)) ||
      // Csc(A) -> 1/sin(A)
      PatternMatching::MatchReplace(e, KCsc(KA), KPow(KSin(KA), -1_e)) ||
      // ArcSec(A) -> acos(1/A)
      PatternMatching::MatchReplace(e, KASec(KA), KACos(KPow(KA, -1_e))) ||
      // ArcCsc(A) -> asin(1/A)
      PatternMatching::MatchReplace(e, KACsc(KA), KASin(KPow(KA, -1_e))) ||
#endif
      false) {
    // e may need to be projected again.
    ShallowSystemProject(e, context);
    return true;
  }

  /* In following replacements, ref node isn't supposed to be replaced with
   * a node needing further projection. */
  return
      // frac(A) -> A - floor(A)
      PatternMatching::MatchReplace(e, KFrac(KA),
                                    KAdd(KA, KMult(-1_e, KFloor(KA)))) ||
      // e -> exp(1)
      PatternMatching::MatchReplace(e, e_e, KExp(1_e)) ||
      // log(A) -> ln(A) * ln(10)^(-1)
      PatternMatching::MatchReplace(
          e, KLog(KA), KMult(KLnUser(KA), KPow(KLn(10_e), -1_e))) ||
      // log(A, B) -> ln(A) * ln(B)^(-1)
      PatternMatching::MatchReplace(
          e, KLogBase(KA, KB), KMult(KLnUser(KA), KPow(KLnUser(KB), -1_e))) ||
      // conj(A) -> re(A)-im(A)*i
      PatternMatching::MatchReplace(e, KConj(KA),
                                    KAdd(KRe(KA), KMult(-1_e, KIm(KA), i_e))) ||
      // - A  -> (-1)*A
      PatternMatching::MatchReplace(e, KOpposite(KA), KMult(-1_e, KA)) ||
      // A - B -> A + (-1)*B
      PatternMatching::MatchReplace(e, KSub(KA, KB),
                                    KAdd(KA, KMult(-1_e, KB))) ||
      // A / B -> A * B^-1
      PatternMatching::MatchReplace(e, KDiv(KA, KB),
                                    KMult(KA, KPow(KB, -1_e))) ||
      // MixedFraction(A + B/C) -> A + B/C
      // TODO assert KB is a simple rational
      PatternMatching::MatchReplace(e, KMixedFraction(KA, KB), KAdd(KA, KB)) ||
      // cos(A) -> trig(A, 0)
      PatternMatching::MatchReplace(e, KCos(KA), KTrig(KA, 0_e)) ||
      // sin(A) -> trig(A, 1)
      PatternMatching::MatchReplace(e, KSin(KA), KTrig(KA, 1_e)) ||
      // tan(A) -> sin(A)/cos(A)
      PatternMatching::MatchReplace(
          e, KTan(KA), KMult(KTrig(KA, 1_e), KPow(KTrig(KA, 0_e), -1_e))) ||
#if POINCARE_TRIGONOMETRY_ADVANCED
      /* acot(A) -> π/2 - atan(A)
      using acos(0) instead of π/2 to handle angle */
      PatternMatching::MatchReplace(e, KACot(KA),
                                    KAdd(KACos(0_e), KMult(-1_e, KATan(KA)))) ||
#endif
#if POINCARE_TRIGONOMETRY_HYPERBOLIC
      // cosh(A) -> cos(i*A)
      PatternMatching::MatchReplace(e, KCosH(KA), KTrig(KMult(KA, i_e), 0_e)) ||
      // sinh(A) -> -i*sin(i*A)
      PatternMatching::MatchReplace(
          e, KSinH(KA), KMult(-1_e, KTrig(KMult(KA, i_e), 1_e), i_e)) ||
      // tanh(A) -> -i*tan(i*A)
      PatternMatching::MatchReplace(
          e, KTanH(KA),
          KMult(-1_e,
                KMult(KTrig(KMult(KA, i_e), 1_e),
                      KPow(KTrig(KMult(KA, i_e), 0_e), -1_e)),
                i_e)) ||
      // ArSinh(A) -> -i*asin(i*A)
      PatternMatching::MatchReplace(
          e, KArSinH(KA), KMult(-1_e, KATrig(KMult(KA, i_e), 1_e), i_e)) ||
      // ArTanh(A) -> -i*atan(i*A)
      PatternMatching::MatchReplace(
          e, KArTanH(KA), KMult(-1_e, KATanRad(KMult(KA, i_e)), i_e)) ||
#endif
#if POINCARE_BOOLEAN
      // A nor B -> not (A or B)
      PatternMatching::MatchReplace(e, KLogicalNor(KA, KB),
                                    KLogicalNot(KLogicalOr(KA, KB))) ||
      // A nand B -> not (A and B)
      PatternMatching::MatchReplace(e, KLogicalNand(KA, KB),
                                    KLogicalNot(KLogicalAnd(KA, KB))) ||
#endif
#if POINCARE_EUCLIDEAN_DIVISION
      // A |- B -> quo(A, B)
      PatternMatching::MatchReplace(e, KEuclideanDiv(KA, KB), KQuo(KA, KB)) ||
#endif
      changed;
}

bool Projection::Expand(Tree* e) {
  PatternMatching::Context ctx;
  if (PatternMatching::Match(e, KATanRad(KA), &ctx) &&
      GetComplexSign(ctx.getTree(KA)).isReal()) {
    // atan(A) -> asin(A/Sqrt(1 + A^2)) for A real
    e->moveTreeOverTree(PatternMatching::CreateSimplify(
        KATrig(
            KMult(KA, KPow(KAdd(1_e, KPow(KA, 2_e)), KMult(-1_e, 1_e / 2_e))),
            1_e),
        ctx));
    return true;
  }
#if POINCARE_TRIGONOMETRY_HYPERBOLIC
  if (PatternMatching::Match(e, KArCosH(KA), &ctx)) {
    ComplexSign signOfChild = GetComplexSign(ctx.getTree(KA));
    if (signOfChild.isReal() && signOfChild.realSign().isPositive()) {
      // ArCosh(A) -> ln(A+sqrt(A^2-1)) for A real and positive
      /* Warning: formula holds for x real and ≥ 1, but it seems to work for x ≥
       * -1 using the principal branches of acosh and ln. TODO: Find a proof of
       * that. */
      e->moveTreeOverTree(PatternMatching::CreateSimplify(
          KLn(KAdd(KA, KPow(KAdd(KPow(KA, 2_e), -1_e), 1_e / 2_e))), ctx));
      return true;
    }
  }
#endif
  return false;
}

}  // namespace Poincare::Internal
