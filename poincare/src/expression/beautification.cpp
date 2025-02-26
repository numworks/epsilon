#include "beautification.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree.h>

#include "advanced_reduction.h"
#include "angle.h"
#include "approximation.h"
#include "arithmetic.h"
#include "context.h"
#include "dependency.h"
#include "division.h"
#include "float.h"
#include "metric.h"
#include "number.h"
#include "projection.h"
#include "rational.h"
#include "symbol.h"
#include "systematic_reduction.h"
#include "units/unit.h"
#include "variables.h"

namespace Poincare::Internal::Beautification {

bool ApplyComplexFormat(Tree* e, Dimension dim,
                        const ProjectionContext& projectionContext);
Tree* GetPolarFormat(const Tree* e, const ProjectionContext& projectionContext);
Tree* GetCartesianFormat(const Tree* e,
                         const ProjectionContext& projectionContext);
bool DeepBeautifyAngleFunctions(Tree* e,
                                const ProjectionContext& projectionContext);
bool BottomUpBeautifyAngleFunctions(Tree* e,
                                    const ProjectionContext& projectionContext,
                                    bool* simplifyParent,
                                    bool canSystematicReduce = true);
bool ShallowBeautifyAngleFunctions(Tree* e, AngleUnit angleUnit,
                                   bool* simplifyParent,
                                   bool canSystematicReduce);
bool ShallowBeautifyPercent(Tree* e);
bool ShallowBeautifyOppositesDivisionsRoots(Tree* e, void* context);
bool ShallowBeautifyPowerOfTangent(Tree* e, void* context);
bool ShallowBeautify(Tree* e, void* context);
bool ShallowBeautifySpecialDisplays(Tree* e, void* context);

float DegreeForSortingAddition(const Tree* e, bool symbolsOnly) {
  switch (e->type()) {
    case Type::Mult: {
      /* If we consider the symbol degree, the degree of a multiplication is
       * the sum of the degrees of its terms :
       * 3*(x^2)*y -> deg = 0+2+1 = 3.
       *
       * If we consider the degree of any term, we choose that the degree of a
       * multiplication is the degree of the most-right term :
       * 4*sqrt(2) -> deg = 0.5.
       *
       * This is to ensure that deg(5) > deg(5*sqrt(3)) and deg(x^4) >
       * deg(x*y^3)
       * */
      if (symbolsOnly) {
        float degree = 0.;
        for (const Tree* c : e->children()) {
          degree += DegreeForSortingAddition(c, symbolsOnly);
        }
        return degree;
      }
      assert(e->numberOfChildren() > 0);
      return DegreeForSortingAddition(e->lastChild(), symbolsOnly);
    }
    case Type::Pow: {
      double baseDegree = DegreeForSortingAddition(e->child(0), symbolsOnly);
      if (baseDegree == 0.) {
        /* We escape here so that even if the exponent is not a number,
         * the degree is still computed to 0.
         * It is useful for 2^ln(3) for example, which has a symbol degree
         * of 0 even if the exponent is not a number.*/
        return 0.;
      }
      const Tree* index = e->child(1);
      if (index->isNumber()) {
        return Approximation::To<float>(index, Approximation::Parameters{}) *
               baseDegree;
      }
      return NAN;
    }
    case Type::UserSymbol:
    case Type::Var:
      return 1.;
    default:
      return symbolsOnly ? 0. : 1.;
  }
}

/* Find and beautify trigonometric system nodes while converting the angles.
 * Simplifications are needed, this has to be done before beautification.
 * A bottom-up pattern is also needed because inverse trigonometric must
 * simplify its parents. */
bool DeepBeautifyAngleFunctions(Tree* e,
                                const ProjectionContext& projectionContext) {
  bool dummy = false;
  /* BottomUpBeautifyAngleFunctions temporarily introduces AngleUnitContext
   * nodes to allow simplification and approximation of beautified angle
   * functions. */
  bool changed = BottomUpBeautifyAngleFunctions(e, projectionContext, &dummy);
  if (changed && projectionContext.m_advanceReduce &&
      projectionContext.m_angleUnit != AngleUnit::Radian) {
    // Expands possibilities may have been added.
    AdvancedReduction::Reduce(e);
    Dependency::DeepRemoveUselessDependencies(e);
  }
  // Remove AngleUnitContext nodes
  e->ApplyShallowTopDown(e, [](Tree* e, void* context) {
    if (e->isAngleUnitContext()) {
      e->removeNode();
      return true;
    }
    return false;
  });
  return changed;
}

bool BottomUpBeautifyAngleFunctions(Tree* e,
                                    const ProjectionContext& projectionContext,
                                    bool* simplifyParent,
                                    bool canSystematicReduce) {
  bool modified = false;
  bool mustSystematicReduce = false;
  // Do not use systematicReduction in dependency lists
  canSystematicReduce &= !e->isDepList();
  for (Tree* child : e->children()) {
    bool tempMustSystematicReduce = false;
    modified |= BottomUpBeautifyAngleFunctions(child, projectionContext,
                                               &tempMustSystematicReduce,
                                               canSystematicReduce);
    mustSystematicReduce |= tempMustSystematicReduce;
  }
  if (ShallowBeautifyAngleFunctions(e, projectionContext.m_angleUnit,
                                    simplifyParent, canSystematicReduce)) {
    modified = true;
  } else if (mustSystematicReduce) {
    assert(modified);
    *simplifyParent = SystematicReduction::ShallowReduce(e);
  }
  return modified;
}

// At this stage of the simplification, advanced reductions are expected.
bool ShallowBeautifyAngleFunctions(Tree* e, AngleUnit angleUnit,
                                   bool* simplifyParent,
                                   bool canSystematicReduce) {
  // Beautify System nodes to prevent future simplifications.
  if (e->isTrig()) {
#if POINCARE_TRIGONOMETRY_HYPERBOLIC
    // Hyperbolic functions
    if (
        // cos(A?*i) -> cosh(A)
        PatternMatching::MatchReplace(e, KTrig(KMult(KA_s, i_e), 0_e),
                                      KCosH(KMult(KA_s)),
                                      canSystematicReduce) ||
        // sin(A?*i) -> sinh(A)*i
        PatternMatching::MatchReplace(e, KTrig(KMult(KA_s, i_e), 1_e),
                                      KMult(KSinH(KMult(KA_s)), i_e),
                                      canSystematicReduce)) {
      // Necessary to simplify i introduced here
      *simplifyParent = true;
      // Hyperbolic trigonometric functions don't need AngleUnitContext
      return true;
    };
#endif
    if (angleUnit != AngleUnit::Radian) {
      Tree* child = e->child(0);
      child->moveTreeOverTree(PatternMatching::Create(
          KMult(KA, KB), {.KA = child, .KB = Angle::RadTo(angleUnit)},
          canSystematicReduce));
      /* This adds new potential multiplication expansions. Another advanced
       * reduction in DeepBeautify may be needed.
       * TODO: Call AdvancedReduction::Reduce in DeepBeautify only if we went
       * here. */
    }
    PatternMatching::MatchReplace(e, KTrig(KA, 0_e), KCos(KA)) ||
        PatternMatching::MatchReplace(e, KTrig(KA, 1_e), KSin(KA));
    e->moveNodeAtNode(SharedTreeStack->pushAngleUnitContext(angleUnit));
    return true;
  }
  if (e->isATrig() || e->isATanRad()) {
#if POINCARE_TRIGONOMETRY_HYPERBOLIC
    // Inverse hyperbolic functions
    if (
        // asin(A?*i) -> asinh(A)*i
        PatternMatching::MatchReplace(e, KATrig(KMult(KA_s, i_e), 1_e),
                                      KMult(KArSinH(KMult(KA_s)), i_e),
                                      canSystematicReduce) ||
        // atan(A?*i) -> atanh(A)*i
        PatternMatching::MatchReplace(e, KATanRad(KMult(KA_s, i_e)),
                                      KMult(KArTanH(KMult(KA_s)), i_e),
                                      canSystematicReduce)) {
      // Necessary to simplify i introduced here
      *simplifyParent = true;
      // Hyperbolic trigonometric functions don't need AngleUnitContext
      return true;
    }
#endif
    PatternMatching::MatchReplace(e, KATrig(KA, 0_e), KACos(KA)) ||
        PatternMatching::MatchReplace(e, KATrig(KA, 1_e), KASin(KA)) ||
        PatternMatching::MatchReplace(e, KATanRad(KA), KATan(KA));
    e->moveNodeAtNode(SharedTreeStack->pushAngleUnitContext(angleUnit));
    if (angleUnit != AngleUnit::Radian) {
      e->moveTreeOverTree(PatternMatching::Create(
          KMult(KA, KB), {.KA = e, .KB = Angle::ToRad(angleUnit)},
          canSystematicReduce));
      *simplifyParent = true;
    }
    return true;
  }
  return false;
}

bool ShallowBeautifyPercent(Tree* e) {
  // A% -> A / 100
  if (PatternMatching::MatchReplace(e, KPercentSimple(KA), KDiv(KA, 100_e))) {
    return true;
  }
  // TODO_PCJ PercentAddition had a deepBeautify to preserve addition order
  PatternMatching::Context ctx;
  if (!PatternMatching::Match(e, KPercentAddition(KA, KB), &ctx)) {
    return false;
  }
  /* ShallowBeautifyPercent comes after Rational(-20) -> Opposite(Rational(20)),
   * we need to revert it partially. */
  // A - B% -> A * (1 - B / 100)
  return PatternMatching::MatchReplace(e, KPercentAddition(KA, KOpposite(KB)),
                                       KMult(KA, KSub(1_e, KDiv(KB, 100_e))))
         // A + B% -> A * (1 + B / 100)
         ||
         PatternMatching::MatchReplace(e, KPercentAddition(KA, KB),
                                       KMult(KA, KAdd(1_e, KDiv(KB, 100_e))));
}

// Turn "m" into "1*m", "m*s" into "1*m*s" and "3vyd+ft" into "3*yd+1*ft".
bool DeepBeautifyUnits(Tree* e) {
#if POINCARE_UNIT
  if (e->isAdd()) {
    bool changed = false;
    for (Tree* child : e->children()) {
      changed = DeepBeautifyUnits(child) || changed;
    }
    return changed;
  }
  if (Units::Unit::IsUnitOrPowerOfUnit(e)) {
    e->cloneNodeAtNode(1_e);
    e->cloneNodeAtNode(KMult.node<2>);
    return true;
  }
  if (e->isMult() && e->numberOfChildren() > 0 &&
      Units::Unit::IsUnitOrPowerOfUnit(e->child(0))) {
    NAry::AddChildAtIndex(e, (1_e)->cloneTree(), 0);
    return true;
  }
#endif
  return false;
}

bool DeepBeautify(Tree* e, ProjectionContext projectionContext) {
  bool changed =
      ApplyComplexFormat(e, projectionContext.m_dimension, projectionContext);
  changed = DeepBeautifyAngleFunctions(e, projectionContext) || changed;
  changed = Tree::ApplyShallowTopDown(e, ShallowBeautify) || changed;
  changed = DeepBeautifyUnits(e) || changed;
  /* Divisions are created after the main beautification since they work top
   * down and require powers to have been built from exponentials already. */
  changed =
      Tree::ApplyShallowTopDown(e, ShallowBeautifyOppositesDivisionsRoots) ||
      changed;
  changed =
      Tree::ApplyShallowTopDown(e, ShallowBeautifyPowerOfTangent) || changed;
  changed =
      Tree::ApplyShallowTopDown(e, ShallowBeautifySpecialDisplays) || changed;
  changed = Variables::BeautifyToName(e) || changed;
  assert(!e->hasDescendantSatisfying(Projection::IsForbidden));
  return changed;
}

bool ShallowBeautifyOppositesDivisionsRoots(Tree* e, void* context) {
  if (e->isMult() && e->numberOfChildren() >= 2 &&
      Dimension::Get(e->child(1)).isUnit() &&
      e->child(0)->isStrictlyNegativeRational()) {
    // (-A)*U -> -A*U, with U a unit
    Rational::SetSign(e->child(0), NonStrictSign::Positive);
    e->cloneNodeAtNode(KOpposite);
    return true;
  } else if (e->isMult() || e->isPow() || e->isRational()) {
    /* Turn multiplications with negative powers into divisions and negative
     * rationals into opposites */
    if (Division::BeautifyIntoDivision(e)) {
      return true;
    }
  }

  // Roots are created along divisions to have x^(-1/2) -> 1/x^(1/2) -> 1/√(x)
  if (e->isPow() && e->child(0)->isEulerE()) {
    // We do not want e^1/2 -> √(e)
    return false;
  }

  // A^(1/2) -> Sqrt(A)
  if (PatternMatching::MatchReplace(e, KPow(KA, 1_e / 2_e), KSqrt(KA))) {
    return true;
  }

  // A^(1/N) -> Root(A, N)
  if (e->isPow() && e->child(1)->isRational() &&
      Rational::Numerator(e->child(1)).isOne()) {
    Tree* root = SharedTreeStack->pushRoot();
    e->child(0)->cloneTree();
    Rational::Denominator(e->child(1)).pushOnTreeStack();
    e->moveTreeOverTree(root);
    return true;
  }

  return false;
}

bool ShallowBeautifyPowerOfTangent(Tree* e, void* context) {
  if (!e->isDiv()) {
    return false;
  }
  bool matchResult =
      // (sin(A))^n/(cos(A))^n -> (tan(A))^n
      PatternMatching::MatchReplace(
          e, KDiv(KMult(KA_s, KPow(KSin(KB), KC), KD_s), KPow(KCos(KB), KC)),
          KMult(KA_s, KPow(KTan(KB), KC), KD_s)) ||
      // (cos(A))^n/(sin(A))^n -> (cot(A))^n
      PatternMatching::MatchReplace(
          e, KDiv(KMult(KA_s, KPow(KCos(KB), KC), KD_s), KPow(KSin(KB), KC)),
          KMult(KA_s, KPow(KCot(KB), KC), KD_s)) ||
      // (sinh(A))^n/(cosh(A))^n -> (tanh(A))^n
      PatternMatching::MatchReplace(
          e, KDiv(KMult(KA_s, KPow(KSinH(KB), KC), KD_s), KPow(KCosH(KB), KC)),
          KMult(KA_s, KPow(KTanH(KB), KC), KD_s));
  // Check that the resulting multiplication is correctly sorted
  assert(!e->isMult() || !NAry::Sort(e, Order::OrderType::Beautification));
  return matchResult;
}

// Reverse most system projections to display better expressions
bool ShallowBeautify(Tree* e, void* context) {
  bool changed = false;
  if (e->isAdd()) {
    NAry::Sort(e, Order::OrderType::AdditionBeautification);
  }

#if 0
  // TODO: handle lnReal too
  // ln(A)      * ln(B)^(-1) -> log(A, B)
  // ln(A)^(-1) * ln(B)      -> log(B, A)
  changed = PatternMatching::MatchReplace(
                ref, KMult(KA_s, KLn(KB), KPow(KLn(KC), -1_e), KD_s),
                KMult(KA_s, KLogBase(KB, KC), KD_s)) ||
            PatternMatching::MatchReplace(
                ref, KMult(KA_s, KPow(KLn(KB), -1_e), KLn(KC), KD_s),
                KMult(KA_s, KLogBase(KC, KB), KD_s));
#endif

  // ln(A)       * ln(10)^(-1) -> log(A)
  // ln(10)^(-1) * ln(A)       -> log(A)
  changed = PatternMatching::MatchReplace(
                e, KMult(KA_s, KLn(KB), KPow(KLn(10_e), -1_e), KC_s),
                KMult(KA_s, KLog(KB), KC_s)) ||
            PatternMatching::MatchReplace(
                e, KMult(KA_s, KPow(KLn(10_e), -1_e), KLn(KB), KC_s),
                KMult(KA_s, KLog(KB), KC_s));

  int n = e->numberOfChildren();
  while (e->isMult() && n > 1 &&
         (
             // sin(A)/cos(A) -> tan(A)
             PatternMatching::MatchReplace(
                 e, KMult(KA_s, KPow(KCos(KB), -1_e), KC_s, KSin(KB), KD_s),
                 KMult(KA_s, KC_s, KTan(KB), KD_s)) ||
#if POINCARE_TRIGONOMETRY_ADVANCED
             // cos(A)/sin(A) -> cot(A)
             PatternMatching::MatchReplace(
                 e, KMult(KA_s, KCos(KB), KC_s, KPow(KSin(KB), -1_e), KD_s),
                 KMult(KA_s, KC_s, KCot(KB), KD_s)) ||
#endif
#if POINCARE_TRIGONOMETRY_HYPERBOLIC
             // sinh(A)/cosh(A) -> tanh(A)
             PatternMatching::MatchReplace(
                 e, KMult(KA_s, KPow(KCosH(KB), -1_e), KC_s, KSinH(KB), KD_s),
                 KMult(KA_s, KC_s, KTanH(KB), KD_s)) ||
#endif
             // Remove 1.0 from multiplications
             PatternMatching::MatchReplace(e, KMult(KA_s, 1.0_de, KB_s),
                                           KMult(KA_s, KB_s)) ||
             PatternMatching::MatchReplace(e, KMult(KA_s, 1.0_fe, KB_s),
                                           KMult(KA_s, KB_s)))) {
    assert(!e->isMult() || n > e->numberOfChildren());
    n = e->numberOfChildren();
    changed = true;
  }

  // PowerReal(A,B) -> A^B
  // exp(A? * ln(B) * C?) -> B^(A*C)
  if (PatternMatching::MatchReplace(e, KPowReal(KA, KB), KPow(KA, KB)) ||
      PatternMatching::MatchReplace(e, KExp(KMult(KA_s, KLn(KB), KC_s)),
                                    KPow(KB, KMult(KA_s, KC_s)))) {
    changed = true;
  }

  if (e->isOfType({Type::Mult, Type::GCD, Type::LCM}) &&
      NAry::Sort(e, Order::OrderType::Beautification)) {
    return true;
  }

  return
      // ln(x) -> lnUser(x)
      PatternMatching::MatchReplace(e, KLn(KA), KLnUser(KA)) ||
      // exp(1) -> e
      PatternMatching::MatchReplace(e, KExp(1_e), e_e) ||
      // exp(A) -> e^A
      PatternMatching::MatchReplace(e, KExp(KA), KPow(e_e, KA)) ||
      // A - floor(A) -> frac(A)
      PatternMatching::MatchReplace(
          e, KAdd(KA_s, KB, KC_s, KMult(-1_e, KFloor(KB)), KD_s),
          KAdd(KA_s, KC_s, KFrac(KB), KD_s)) ||
      changed;
}

bool ApplyComplexFormat(Tree* e, Dimension dim,
                        const ProjectionContext& projectionContext) {
  ComplexFormat format = projectionContext.m_complexFormat;
  if (e->isUndefined() || e->isFactor() || format == ComplexFormat::Real) {
    return false;
  }
  assert(format == ComplexFormat::Cartesian || format == ComplexFormat::Polar);
  // Apply element-wise on explicit lists, matrices, sets.
  if (e->isMatrix() || e->isSet() || (dim.isScalar() && e->isList())) {
    bool changed = false;
    for (Tree* child : e->children()) {
      assert(Dimension::Get(child).isScalar());
      changed |=
          ApplyComplexFormat(child, Dimension::Scalar(), projectionContext);
      if (e->isDep()) {
        // Skip DepList
        break;
      }
    }
    // Bubble up any dependencies that have appeared
    changed = Dependency::ShallowBubbleUpDependencies(e) || changed;
    return changed;
  }
  if (!dim.isScalar()) {
    return false;
  }
  assert(!e->isDepList());
  Tree* result =
      (format == ComplexFormat::Polar ? GetPolarFormat : GetCartesianFormat)(
          e, projectionContext);
  /* If complex format is different, apply it if :
   *  - It has a better metric than the original expression
   *  - It simplified all the introduced re/im/arg/abs nodes */
  if (result && !Dependency::MainTreeIsIdenticalToMain(result, e) &&
      (Metric::GetMetric(e) >= Metric::GetMetric(result) ||
       !Dependency::SafeMain(result)->hasDescendantSatisfying(
           [](const Tree* e) {
             return e->isRe() || e->isIm() || e->isArg() || e->isAbs();
           }))) {
    e->moveTreeOverTree(result);
    return true;
  }
  if (result != nullptr) {
    result->removeTree();
  }
  return false;
}

Tree* GetPolarFormat(const Tree* e,
                     const ProjectionContext& projectionContext) {
  /* If the expression comes from an approximation, its polar form must stay an
   * approximation. Arg system reductions tends to remove approximated nodes, so
   * we need to re-approximate them. */
  bool hasDoubleFloats = false, hasSingleFloats = false;
  if (e->hasDescendantSatisfying([](const Tree* e) { return e->isFloat(); })) {
    hasSingleFloats = e->hasDescendantSatisfying(
        [](const Tree* e) { return e->isSingleFloat(); });
    hasDoubleFloats = !hasSingleFloats;
  }
  /* Try to turn a scalar x into abs(x)*e^(i×arg(x))
   * If abs or arg stays unreduced, leave x as it was. */
  Tree* result = SharedTreeStack->pushMult(2);
  Tree* abs = SharedTreeStack->pushAbs();
  e->cloneTree();
  bool hasReduced = SystematicReduction::ShallowReduce(abs);
  if (hasReduced && Dependency::MainTreeIsIdenticalToMain(e, abs)) {
    // Early escape positive real expressions
    abs->removeTree();
    result->removeNode();
    return nullptr;
  }
  if (projectionContext.m_advanceReduce) {
    hasReduced = AdvancedReduction::Reduce(abs) || hasReduced;
  }
  if (!hasReduced) {
    abs->removeTree();
    result->removeNode();
    return nullptr;
  }
  /* If this assert fails, an approximated node has been lost during systematic
   * simplification, ApproximateAndReplaceEveryScalar should be called on abs
   * like below with arg. */
  assert(
      !(hasSingleFloats || hasDoubleFloats) ||
      abs->hasDescendantSatisfying([](const Tree* e) { return e->isFloat(); }));
  Tree* exp = SharedTreeStack->pushExp();
  Tree* mult = SharedTreeStack->pushMult(2);
  Tree* arg = SharedTreeStack->pushArg();
  e->cloneTree();
  // Both abs and arg must have been reduced
  hasReduced = SystematicReduction::ShallowReduce(arg);
  if (projectionContext.m_advanceReduce) {
    hasReduced = AdvancedReduction::Reduce(arg) || hasReduced;
  }
  if (hasSingleFloats) {
    Approximation::ApproximateAndReplaceEveryScalar<float>(arg);
  } else if (hasDoubleFloats) {
    Approximation::ApproximateAndReplaceEveryScalar<double>(arg);
  }
  SharedTreeStack->pushComplexI();
  if (!hasReduced) {
    result->removeTree();
    return nullptr;
  }
  /* exp is not ShallowReduced to preserve exp(A*i) form with A within ]-π,π]
   * because of exp(arg(exp(A*i))*i) -> exp(A*i) reduction. */
  // Bubble up dependencies that appeared during reduction.
  bool bubbledUpDependencies = Dependency::ShallowBubbleUpDependencies(mult) &&
                               Dependency::ShallowBubbleUpDependencies(exp);
  // Invalidate Tree pointers that are unused and may get corrupted
  if (bubbledUpDependencies) {
    arg = nullptr;
  }
  mult = nullptr;
  bubbledUpDependencies =
      Dependency::ShallowBubbleUpDependencies(result) || bubbledUpDependencies;
  exp = nullptr;
  if (bubbledUpDependencies) {
    // abs and arg pointer are preserved if no dependencies are bubbled up
    arg = nullptr;
    abs = nullptr;
    Dependency::DeepRemoveUselessDependencies(result);
  }
  Tree* polarForm = result->isDep() ? Dependency::Main(result) : result;
  if (polarForm->isUndef()) {
    polarForm->removeTree();
    return nullptr;
  }
  bool argIsNull = false;
  if (bubbledUpDependencies) {
    // abs and arg pointers may have been invalidated, find them again.
    PatternMatching::Context ctx;
    [[maybe_unused]] bool find = PatternMatching::Match(
        polarForm, KMult(KA, KExp(KMult(KB_s, i_e))), &ctx);
    assert(find);
    abs = const_cast<Tree*>(ctx.getTree(KA));
    argIsNull =
        ctx.getNumberOfTrees(KB) == 1 && Number::IsNull(ctx.getTree(KB));
  } else {
    argIsNull = Number::IsNull(arg);
  }
  /* arg may be a multiplication, flatten the parent multiplication after having
   * bubbled up dependencies. */
  assert(polarForm->child(1)->isExp() &&
         polarForm->child(1)->child(0)->isMult());
  NAry::Flatten(polarForm->child(1)->child(0));
  if (Number::IsNull(abs) || argIsNull) {
    NAry::RemoveChildAtIndex(polarForm, 1);
  }
  if (abs->isOne()) {
    NAry::RemoveChildAtIndex(polarForm, 0);
  }
  NAry::SquashIfPossible(polarForm);
  return result;
}

Tree* GetCartesianFormat(const Tree* e,
                         const ProjectionContext& projectionContext) {
  /* Try to turn a scalar x into re(x)+i×im(x)
   * If re or im stays unreduced, leave x as it was. */
  Tree* result = SharedTreeStack->pushAdd(2);
  Tree* re = SharedTreeStack->pushRe();
  e->cloneTree();
  bool hasReduced = SystematicReduction::ShallowReduce(re);
  if (hasReduced && Dependency::MainTreeIsIdenticalToMain(e, re)) {
    // Early escape real expressions
    re->removeTree();
    result->removeNode();
    return nullptr;
  }
  if (projectionContext.m_advanceReduce) {
    hasReduced = AdvancedReduction::Reduce(re) || hasReduced;
  }
  if (!hasReduced) {
    re->removeTree();
    result->removeNode();
    return nullptr;
  }
  Tree* mult = SharedTreeStack->pushMult(2);
  Tree* im = SharedTreeStack->pushIm();
  e->cloneTree();
  // Both im and re must have been reduced
  hasReduced = SystematicReduction::ShallowReduce(im);
  if (projectionContext.m_advanceReduce) {
    hasReduced = AdvancedReduction::Reduce(im) || hasReduced;
  }
  SharedTreeStack->pushComplexI();
  if (!hasReduced) {
    result->removeTree();
    return nullptr;
  }
  // Bubble up dependencies that appeared during reduction.
  bool bubbledUpDependencies = Dependency::ShallowBubbleUpDependencies(mult);
  if (bubbledUpDependencies) {
    // Invalidate Tree pointers that may get corrupted
    im = nullptr;
  }
  bubbledUpDependencies =
      Dependency::ShallowBubbleUpDependencies(result) || bubbledUpDependencies;
  if (bubbledUpDependencies) {
    mult = nullptr;
    re = nullptr;
    Dependency::DeepRemoveUselessDependencies(result);
  }
  Tree* cartesianForm = result->isDep() ? Dependency::Main(result) : result;
  if (cartesianForm->isUndef()) {
    cartesianForm->removeTree();
    return nullptr;
  }
  if (bubbledUpDependencies) {
    // im and re pointers may have been invalidated, find them again.
    PatternMatching::Context ctx;
    [[maybe_unused]] bool find =
        PatternMatching::Match(cartesianForm, KAdd(KA, KMult(KB, i_e)), &ctx);
    assert(find);
    re = const_cast<Tree*>(ctx.getTree(KA));
    im = const_cast<Tree*>(ctx.getTree(KB));
    mult = re->nextTree();
  }

  if (Number::IsNull(im)) {
    mult->removeTree();
    cartesianForm->removeNode();
  } else {
    if (Number::IsOne(im)) {
      im->removeTree();
      mult->removeNode();
    } else {
      NAry::Flatten(mult);
    }
    if (Number::IsNull(re)) {
      re->removeTree();
      cartesianForm->removeNode();
    }
  }

  return result;
}

template <typename T>
Tree* PushBeautifiedComplex(std::complex<T> value,
                            ComplexFormat complexFormat) {
  // TODO: factorize with beautification somehow ?
  T re = value.real(), im = value.imag();
  if (std::isnan(re) || std::isnan(im)) {
    return Approximation::IsNonReal(value) ? KNonReal->cloneTree()
                                           : KUndef->cloneTree();
  }
  assert(complexFormat != ComplexFormat::None);
  assert(im == 0 || complexFormat != ComplexFormat::Real);
  if (im == 0 && (complexFormat != ComplexFormat::Polar || re >= 0)) {
    return SharedTreeStack->pushFloat(re);
  }
  Tree* result = Tree::FromBlocks(SharedTreeStack->lastBlock());
  // Real part and separator
  if (complexFormat == ComplexFormat::Cartesian) {
    // [re+]
    if (re != 0) {
      SharedTreeStack->pushAdd(2);
      SharedTreeStack->pushFloat(re);
    }
  } else {
    // [abs×]e^
    T abs = std::abs(value);
    if (abs != 1) {
      SharedTreeStack->pushMult(2);
      SharedTreeStack->pushFloat(abs);
    }
    SharedTreeStack->pushPow();
    SharedTreeStack->pushEulerE();
    im = std::arg(value);
  }
  // Complex part ±[im×]i
  if (im < 0) {
    SharedTreeStack->pushOpposite();
    im = -im;
  }
  if (im != 1) {
    SharedTreeStack->pushMult(2);
    SharedTreeStack->pushFloat(im);
  }
  SharedTreeStack->pushComplexI();
  return result;
}

bool ShallowBeautifySpecialDisplays(Tree* e, void* context) {
  return Arithmetic::BeautifyFactor(e) || ShallowBeautifyPercent(e);
}

template Tree* PushBeautifiedComplex(std::complex<float>,
                                     ComplexFormat complexFormat);
template Tree* PushBeautifiedComplex(std::complex<double>,
                                     ComplexFormat complexFormat);

}  // namespace Poincare::Internal::Beautification
