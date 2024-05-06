#include "projection.h"

#include <apps/global_preferences.h>
#include <poincare/preferences.h>
#include <poincare/src/memory/pattern_matching.h>

#include "angle.h"
#include "decimal.h"
#include "physical_constant.h"
#include "symbol.h"
#include "variables.h"

namespace Poincare::Internal {

bool Projection::DeepReplaceUserNamed(Tree* tree, ProjectionContext ctx) {
  if (ctx.m_symbolic == SymbolicComputation::DoNotReplaceAnySymbol) {
    return false;
  }
  bool changed = false;
  /* ShallowReplaceUserNamed may push and remove trees at the end of TreeStack.
   * We push a temporary tree to preserve TreeRef.
   * TODO: Maybe find a solution for this unintuitive workaround */
  TreeRef nextTree = tree->nextTree()->cloneTreeBeforeNode(0_e);
  while (tree->block() < nextTree->block()) {
    if (tree->isParametric()) {
      // Skip Parametric node and its variable, never replaced.
      static_assert(Parametric::k_variableIndex == 0);
      tree = tree->nextNode()->nextTree();
    }
    changed = ShallowReplaceUserNamed(tree, ctx) || changed;
    tree = tree->nextNode();
  }
  nextTree->removeTree();
  return changed;
}

bool Projection::ShallowReplaceUserNamed(Tree* tree, ProjectionContext ctx) {
  SymbolicComputation symbolic = ctx.m_symbolic;
  assert(symbolic != SymbolicComputation::DoNotReplaceAnySymbol);
  bool treeIsUserFunction = tree->isUserFunction();
  if (!treeIsUserFunction) {
    if (!tree->isUserSymbol() ||
        symbolic ==
            SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions) {
      return false;
    }
  }
  if (symbolic == SymbolicComputation::ReplaceAllSymbolsWithUndefined) {
    tree->cloneTreeOverTree(KNotDefined);
    return true;
  }
  // Get Definition
  const Tree* definition =
      ctx.m_context ? ctx.m_context->treeForSymbolIdentifier(
                          Symbol::GetName(tree), Symbol::Length(tree),
                          treeIsUserFunction
                              ? Poincare::Context::SymbolAbstractType::Function
                              : Poincare::Context::SymbolAbstractType::Symbol)
                    : nullptr;
  if (symbolic ==
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined &&
      !definition) {
    tree->cloneTreeOverTree(KNotDefined);
    return true;
  } else if (!definition) {
    return false;
  }
  // Otherwise, local variable scope should be handled.
  assert(!Variables::HasVariables(definition));
  // Replace function's symbol with definition
  TreeRef evaluateAt;
  if (treeIsUserFunction) {
    evaluateAt = tree->child(0)->clone();
  }
  tree->cloneTreeOverTree(definition);
  if (treeIsUserFunction) {
    tree->deepReplaceWith(KUnknownSymbol, evaluateAt);
    evaluateAt->removeTree();
  }
  // Replace node again in case it has been replaced with another symbol
  ShallowReplaceUserNamed(tree, ctx);
  return true;
}

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

bool Projection::DeepSystemProject(Tree* e,
                                   ProjectionContext projectionContext) {
  bool changed = false;
  if (projectionContext.m_strategy == Strategy::ApproximateToFloat) {
    changed =
        Approximation::ApproximateAndReplaceEveryScalar(e, &projectionContext);
  }
  return Tree::ApplyShallowInDepth(e, ShallowSystemProject,
                                   &projectionContext) ||
         changed;
}

/* The order of nodes in NAry is not a concern here. They will be sorted before
 * SystemReduction. */
bool Projection::ShallowSystemProject(Tree* e, void* context) {
  /* TODO: Most of the projections could be optimized by simply replacing and
   * inserting nodes. This optimization could be applied in matchAndReplace. See
   * comment in matchAndReplace. */
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);

  bool changed = false;
  if (e->isUndefined()) {
    return changed;
  }
  if (e->isUserSymbol() &&
      projectionContext->m_complexFormat == ComplexFormat::Real &&
      Symbol::GetComplexSign(e) == ComplexSign::Unknown()) {
    Symbol::SetComplexSign(e, ComplexSign::RealUnknown());
    changed = true;
  }
  if (e->isParenthesis()) {
    e->removeNode();
    ShallowSystemProject(e, context);
    return true;
  }
  if (e->isUnit()) {
    Units::Unit::RemoveUnit(e);
    changed = true;
  }
  if (e->isPhysicalConstant()) {
    Tree* value = SharedTreeStack->push<Type::DoubleFloat>(
        PhysicalConstant::GetProperties(e).m_value);
    e->moveTreeOverTree(value);
    return true;
  }

  if (e->isDecimal()) {
    Decimal::Project(e);
    changed = true;
  }

  // Project angles depending on context
  Internal::AngleUnit angleUnit = projectionContext->m_angleUnit;
  if (e->isOfType({Type::Sin, Type::Cos, Type::Tan}) &&
      angleUnit != Internal::AngleUnit::Radian) {
    Tree* child = e->child(0);
    child->moveTreeOverTree(PatternMatching::Create(
        KMult(KA, KB), {.KA = child, .KB = Angle::ToRad(angleUnit)}));
    changed = true;
  } else if (e->isOfType({Type::ASin, Type::ACos, Type::ATan})) {
    /* Project inverse trigonometric functions here to avoid infinite projection
     * to radian loop. */
    // acos(A) -> atrig(A, 0)
    PatternMatching::MatchReplace(e, KACos(KA), KATrig(KA, 0_e)) ||
        // asin(A) -> atrig(A, 1)
        PatternMatching::MatchReplace(e, KASin(KA), KATrig(KA, 1_e)) ||
        // atan(A) -> atanRad(A)
        PatternMatching::MatchReplace(e, KATan(KA), KATanRad(KA));
    if (angleUnit != Internal::AngleUnit::Radian) {
      // arccos_degree(x) = arccos_radians(x) * 180/π
      e->moveTreeOverTree(PatternMatching::Create(
          KMult(KA, KB), {.KA = e, .KB = Angle::RadTo(angleUnit)}));
    }
    return true;
  }

  // inf -> Float(inf) to prevent inf-inf from being 0
  if (e->isInf()) {
    /* TODO: Infinity is only handled as float for now. This can introduce float
     * nodes when strategy is still Default, but should be properly handled by
     * float contamination. Ideally, handle exact inf (∞-∞, ∞^0, 0+, 0-, ...)
     * and remove this. */
    return Approximation::ApproximateAndReplaceEveryScalar(e,
                                                           projectionContext);
  }

  // Under Real complex format, use node alternative to properly handle nonreal.
  bool realMode = projectionContext->m_complexFormat == ComplexFormat::Real;
  if (e->isPow()) {
    if (PatternMatching::MatchReplace(e, KPow(e_e, KA), KExp(KA))) {
    } else if (Dimension::GetDimension(e->child(0)).isMatrix()) {
      e->cloneNodeOverNode(KPowMatrix);
    } else if (realMode) {
      e->cloneNodeOverNode(KPowReal);
    } else {
      return changed;
    }
    return true;
  }

  if (realMode && e->isLn()) {
    e->cloneNodeOverNode(KLnReal);
    return true;
  }

  if (  // Sqrt(A) -> A^0.5
      PatternMatching::MatchReplace(e, KSqrt(KA), KPow(KA, 1_e / 2_e)) ||
      // NthRoot(A, B) -> A^(1/B)
      PatternMatching::MatchReplace(e, KRoot(KA, KB),
                                    KPow(KA, KPow(KB, -1_e))) ||
      // log(A, e) -> ln(e)
      PatternMatching::MatchReplace(e, KLogarithm(KA, e_e), KLn(KA)) ||
      // Sec(A) -> 1/cos(A)
      PatternMatching::MatchReplace(e, KSec(KA), KPow(KCos(KA), -1_e)) ||
      // Csc(A) -> 1/sin(A)
      PatternMatching::MatchReplace(e, KCsc(KA), KPow(KSin(KA), -1_e)) ||
      // ArcSec(A) -> acos(1/A)
      PatternMatching::MatchReplace(e, KASec(KA), KACos(KPow(KA, -1_e))) ||
      // ArcCsc(A) -> asin(1/A)
      PatternMatching::MatchReplace(e, KACsc(KA), KASin(KPow(KA, -1_e))) ||
      // ArCosh(A) -> ln(A+sqrt(A-1)*sqrt(A+1))
      PatternMatching::MatchReplace(
          e, KArCosH(KA),
          KLn(KAdd(KA, KMult(KSqrt(KAdd(KA, -1_e)), KSqrt(KAdd(KA, 1_e)))))) ||
      // ArSinh(A) -> ln(A+sqrt(A^2+1))
      PatternMatching::MatchReplace(
          e, KArSinH(KA), KLn(KAdd(KA, KSqrt(KAdd(KPow(KA, 2_e), 1_e)))))) {
    // Ref node may need to be projected again.
    ShallowSystemProject(e, context);
    return true;
  }

  /* In following replacements, ref node isn't supposed to be replaced with
   * a node needing further projection. */
  return
      // ceil(A)  -> -floor(-A)
      PatternMatching::MatchReplace(e, KCeil(KA),
                                    KMult(-1_e, KFloor(KMult(-1_e, KA)))) ||
      // frac(A) -> A - floor(A)
      PatternMatching::MatchReplace(e, KFrac(KA),
                                    KAdd(KA, KMult(-1_e, KFloor(KA)))) ||
      // e -> exp(1)
      PatternMatching::MatchReplace(e, e_e, KExp(1_e)) ||
      // conj(A) -> re(A)-i*re(A)
      PatternMatching::MatchReplace(e, KConj(KA),
                                    KAdd(KRe(KA), KMult(-1_e, i_e, KIm(KA)))) ||
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
      // tan(A) -> tanRad(A, 1)
      PatternMatching::MatchReplace(e, KTan(KA), KTanRad(KA)) ||
      // log(A) -> ln(A) * ln(10)^(-1)
      PatternMatching::MatchReplace(e, KLog(KA),
                                    KMult(KLn(KA), KPow(KLn(10_e), -1_e))) ||
      // log(A, B) -> ln(A) * ln(B)^(-1)
      PatternMatching::MatchReplace(e, KLogarithm(KA, KB),
                                    KMult(KLn(KA), KPow(KLn(KB), -1_e))) ||
      // Cot(A) -> cos(A)/sin(A) (Avoid tan to for dependencies)
      PatternMatching::MatchReplace(e, KCot(KA),
                                    KMult(KCos(KA), KPow(KSin(KA), -1_e))) ||
      /* ArcCot(A) -> { π/2 if A is 0, atan(1/A) otherwise } using acos(0)
       * instead of π/2 to handle angle unit */
      PatternMatching::MatchReplace(
          e, KACot(KA),
          KPiecewise(KACos(0_e), KEqual(KA, 0_e), KATan(KPow(KA, -1_e)))) ||
      // Cosh(A) -> (exp(A)+exp(-A))*1/2
      PatternMatching::MatchReplace(
          e, KCosH(KA),
          KMult(1_e / 2_e, KAdd(KExp(KA), KExp(KMult(-1_e, KA))))) ||
      // Sinh(A) -> (exp(A)-exp(-A))*1/2
      PatternMatching::MatchReplace(
          e, KSinH(KA),
          KMult(1_e / 2_e,
                KAdd(KExp(KA), KMult(-1_e, KExp(KMult(-1_e, KA)))))) ||
      // Tanh(A) -> (exp(2A)-1)/(exp(2A)+1)
      PatternMatching::MatchReplace(
          e, KTanH(KA),
          KMult(KAdd(KExp(KMult(2_e, KA)), -1_e),
                KPow(KAdd(KExp(KMult(2_e, KA)), 1_e), -1_e))) ||
      // ArTanh(A) -> (ln(1+A)-ln(1-A))*1/2
      PatternMatching::MatchReplace(
          e, KArTanH(KA),
          KMult(1_e / 2_e,
                KAdd(KLn(KAdd(1_e, KA)),
                     KMult(-1_e, KLn(KAdd(1_e, KMult(-1_e, KA))))))) ||
      // A nor B -> not (A or B)
      PatternMatching::MatchReplace(e, KLogicalNor(KA, KB),
                                    KLogicalNot(KLogicalOr(KA, KB))) ||
      // A nand B -> not (A and B)
      PatternMatching::MatchReplace(e, KLogicalNand(KA, KB),
                                    KLogicalNot(KLogicalAnd(KA, KB))) ||
      changed;
}

bool Projection::Expand(Tree* tree) {
  return
      // tan(A) -> sin(A) * cos(A)^(-1)
      PatternMatching::MatchReplaceSimplify(
          tree, KTanRad(KA),
          KMult(KTrig(KA, 1_e), KPow(KTrig(KA, 0_e), -1_e))) ||
      // TODO_PCJ: This expansion introduces KPow when KPowReal could be needed.
      // atan(A) -> asin(A/Sqrt(1 + A^2))
      PatternMatching::MatchReplaceSimplify(
          tree, KATanRad(KA),
          KATrig(
              KMult(KA, KPow(KAdd(1_e, KPow(KA, 2_e)), KMult(-1_e, 1_e / 2_e))),
              1_e));
}

}  // namespace Poincare::Internal
