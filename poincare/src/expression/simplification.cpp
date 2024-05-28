#include "simplification.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/probability/distribution_method.h>

#include "advanced_simplification.h"
#include "approximation.h"
#include "beautification.h"
#include "binary.h"
#include "comparison.h"
#include "dependency.h"
#include "derivation.h"
#include "dimension.h"
#include "infinity.h"
#include "k_tree.h"
#include "list.h"
#include "matrix.h"
#include "number.h"
#include "random.h"
#include "rational.h"
#include "systematic_operation.h"
#include "undefined.h"
#include "unit.h"
#include "variables.h"
#include "vector.h"

namespace Poincare::Internal {

bool Simplification::DeepSystematicReduce(Tree* u) {
  /* Although they are also flattened in ShallowSystematicReduce, flattening
   * here could save multiple ShallowSystematicReduce and flatten calls. */
  bool modified = (u->isMult() || u->isAdd()) && NAry::Flatten(u);
  // Never simplify any dependencies
  if (!u->isSet()) {
    for (Tree* child : u->children()) {
      modified |= DeepSystematicReduce(child);
    }
  }

#if ASSERTIONS
  TreeRef previousTree = u->clone();
#endif
  bool shallowModified = ShallowSystematicReduce(u);
#if ASSERTIONS
  assert(shallowModified != u->treeIsIdenticalTo(previousTree));
  previousTree->removeTree();
#endif
  return shallowModified || modified;
}

/* Approximate all children if one of them is already float. Return true if the
 * entire tree have been approximated. */
bool Simplification::CanApproximateTree(Tree* u, bool* changed) {
  if (u->hasChildSatisfying([](const Tree* e) { return e->isFloat(); }) &&
      Approximation::ApproximateAndReplaceEveryScalar(u)) {
    *changed = true;
    if (u->isFloat()) {
      return true;
    }
  }
  return false;
}

bool Simplification::BubbleUpFromChildren(Tree* u) {
  /* Before systematic reduction, look for things to bubble-up in children. At
   * this step, only children have been shallowReduced. By doing this before
   * shallowReduction, we don't have to handle undef, float and dependency
   * children in specialized systematic reduction. */
  bool bubbleUpFloat = false, bubbleUpDependency = false, bubbleUpUndef = false,
       bubbleUpInf = false;
  for (const Tree* child : u->children()) {
    if (child->isFloat()) {
      bubbleUpFloat = true;
    } else if (child->isDependency()) {
      bubbleUpDependency = true;
    } else if (child->isUndefined()) {
      bubbleUpUndef = true;
    } else if (Infinity::IsPlusOrMinusInfinity(child)) {
      bubbleUpInf = true;
    }
  }

  if (bubbleUpUndef && Undefined::ShallowBubbleUpUndef(u)) {
    ShallowSystematicReduce(u);
    return true;
  }

  if (bubbleUpFloat && Approximation::ApproximateAndReplaceEveryScalar(u)) {
    ShallowSystematicReduce(u);
    return true;
  }

  if (bubbleUpDependency && Dependency::ShallowBubbleUpDependencies(u)) {
    assert(u->isDependency());
    /* u->child(0) may now be reduced again. This could unlock further
     * simplifications. */
    ShallowSystematicReduce(u->child(0)) && ShallowSystematicReduce(u);
    return true;
  }

  if (bubbleUpInf && Infinity::ShallowBubbleUpInfinity(u)) {
    ShallowSystematicReduce(u);
    return true;
  }

  return false;
}

bool Simplification::ShallowSystematicReduce(Tree* u) {
  bool changed = BubbleUpFromChildren(u);
  return SimplifySwitch(u) || changed;
}

bool Simplification::SimplifySwitch(Tree* u) {
  // This assert is quite costly, should be an assert level 2 ?
  assert(Dimension::DeepCheckDimensions(u));
  if (!u->isNAry() && u->numberOfChildren() == 0) {
    // No childless tree have a reduction pattern.
    return false;
  }
  switch (u->type()) {
    case Type::Abs:
      return SimplifyAbs(u);
    case Type::Add:
      return SystematicOperation::SimplifyAddition(u);
    case Type::ATanRad:
      return Trigonometry::SimplifyArcTangentRad(u);
    case Type::ATrig:
      return Trigonometry::SimplifyATrig(u);
    case Type::Binomial:
      return Arithmetic::SimplifyBinomial(u);
    case Type::Arg:
      return SimplifyComplexArgument(u);
    case Type::Diff:
    case Type::NthDiff:
      return Derivation::ShallowSimplify(u);
    case Type::Dim:
      return SimplifyDim(u);
    case Type::Distribution:
      return SimplifyDistribution(u);
    case Type::Exp:
      return SimplifyExp(u);
    case Type::Fact:
      return Arithmetic::SimplifyFactorial(u);
    case Type::Factor:
      return Arithmetic::SimplifyFactor(u);
    case Type::Floor:
      return Arithmetic::SimplifyFloor(u);
    case Type::GCD:
      return Arithmetic::SimplifyGCD(u);
    case Type::Im:
    case Type::Re:
      return SimplifyComplexPart(u);
    case Type::LCM:
      return Arithmetic::SimplifyLCM(u);
    case Type::ListSort:
    case Type::Median:
      return List::ShallowApplyListOperators(u);
    case Type::Ln:
      return Logarithm::SimplifyLn(u);
    case Type::LnReal:
      return SimplifyLnReal(u);
    case Type::Mult:
      return SystematicOperation::SimplifyMultiplication(u);
    case Type::Permute:
      return Arithmetic::SimplifyPermute(u);
    case Type::Piecewise:
      return Binary::SimplifyPiecewise(u);
    case Type::Pow:
      return SimplifyPower(u);
    case Type::PowReal:
      return SimplifyPowerReal(u);
    case Type::Quo:
    case Type::Rem:
      return Arithmetic::SimplifyQuotientOrRemainder(u);
    case Type::Round:
      return Arithmetic::SimplifyRound(u);
    case Type::Sign:
      return SimplifySign(u);
    case Type::Sum:
    case Type::Product:
      return Parametric::SimplifySumOrProduct(u);
    case Type::Trig:
      return Trigonometry::SimplifyTrig(u);
    case Type::TrigDiff:
      return Trigonometry::SimplifyTrigDiff(u);
    default:
      if (u->type().isListToScalar()) {
        return List::ShallowApplyListOperators(u);
      }
      if (u->type().isLogicalOperator()) {
        return Binary::SimplifyBooleanOperator(u);
      }
      if (u->type().isComparison()) {
        return Binary::SimplifyComparison(u);
      }
      if (u->isAMatrixOrContainsMatricesAsChildren()) {
        return Matrix::SimplifySwitch(u);
      }
      return false;
  }
}

bool Simplification::SimplifyDim(Tree* u) {
  Dimension dim = Dimension::GetDimension(u->child(0));
  if (dim.isMatrix()) {
    Tree* result = SharedTreeStack->push<Type::Matrix>(1, 2);
    Integer::Push(dim.matrix.rows);
    Integer::Push(dim.matrix.cols);
    u->moveTreeOverTree(result);
    return true;
  }
  return List::ShallowApplyListOperators(u);
}

bool Simplification::SimplifyExp(Tree* u) {
  Tree* child = u->child(0);
  if (child->isLn()) {
    /* TODO_PCJ: Add a ln(x) dependency on user-inputted ln only when x can be
     * null. */
    // exp(ln(x)) -> x
    u->removeNode();
    u->removeNode();
    return true;
  }
  if (child->isZero()) {
    // exp(0) = 1
    u->cloneTreeOverTree(1_e);
    return true;
  }
  PatternMatching::Context ctx;
  if (PatternMatching::Match(KExp(KMult(KA, KLn(KB))), u, &ctx) &&
      (ctx.getNode(KA)->isInteger() || ctx.getNode(KB)->isZero())) {
    /* To ensure there is only one way of representing x^n. Also handle 0^y with
     * Power logic. */
    // exp(n*ln(x)) -> x^n with n an integer or x null.
    u->moveTreeOverTree(PatternMatching::CreateSimplify(KPow(KB, KA), ctx));
    assert(!u->isExp());
    return true;
  }
  return false;
}

bool Simplification::SimplifyAbs(Tree* u) {
  assert(u->isAbs());
  Tree* child = u->child(0);
  if (child->isAbs()) {
    // ||x|| -> |x|
    child->removeNode();
    assert(!SimplifyAbs(u));
    return true;
  }
  ComplexSign complexSign = ComplexSign::Get(child);
  if (!complexSign.isPure()) {
    return false;
  }
  bool isReal = complexSign.isReal();
  Sign sign = isReal ? complexSign.realSign() : complexSign.imagSign();
  if (sign.canBeStriclyNegative() && sign.canBeStriclyPositive()) {
    return false;
  }
  const Tree* minusOne = (isReal == sign.canBeStriclyNegative()) ? -1_e : 1_e;
  const Tree* complexI = isReal ? 1_e : i_e;
  // |3| = |-3| = |3i| = |-3i| = 3
  u->moveTreeOverTree(PatternMatching::CreateSimplify(
      KMult(KA, KB, KC), {.KA = minusOne, .KB = complexI, .KC = child}));
  return true;
}

bool Simplification::SimplifyPower(Tree* u) {
  assert(u->isPow());
  Tree* v = u->child(0);
  // 1^x -> 1
  if (v->isOne()) {
    u->cloneTreeOverTree(1_e);
    return true;
  }
  // v^n
  TreeRef n = v->nextTree();
  if (v->isZero()) {
    ComplexSign indexSign = ComplexSign::Get(n);
    if (indexSign.realSign().isStrictlyPositive()) {
      // 0^x is always defined.
      u->cloneTreeOverTree(0_e);
      return true;
    }
    if (!indexSign.realSign().canBeStriclyPositive()) {
      // 0^x cannot be defined
      u->cloneTreeOverTree(KOutOfDefinition);
      return true;
    }
    // Use a dependency as a fallback.
    return PatternMatching::MatchReplace(u, KA, KDep(0_e, KSet(KA)));
  }
  // After systematic reduction, a power can only have integer index.
  if (!n->isInteger()) {
    // v^n -> exp(n*ln(v))
    return PatternMatching::MatchReplaceSimplify(u, KPow(KA, KB),
                                                 KExp(KMult(KLn(KA), KB)));
  }
  if (v->isRational()) {
    u->moveTreeOverTree(Rational::IntegerPower(v, n));
    return true;
  }
  // v^0 -> 1
  if (n->isZero()) {
    if (ComplexSign::Get(v).canBeNull()) {
      return PatternMatching::MatchReplace(u, KA, KDep(1_e, KSet(KA)));
    }
    u->cloneTreeOverTree(1_e);
    return true;
  }
  // v^1 -> v
  if (n->isOne()) {
    u->moveTreeOverTree(v);
    return true;
  }
  if (v->isComplexI()) {
    // i^n -> ±1 or ±i
    Tree* remainder =
        IntegerHandler::Remainder(Integer::Handler(n), IntegerHandler(4));
    int rem = Integer::Handler(remainder).to<uint8_t>();
    remainder->removeTree();
    u->cloneTreeOverTree(
        rem == 0 ? 1_e
                 : (rem == 1 ? i_e : (rem == 2 ? -1_e : KMult(-1_e, i_e))));
    return true;
  }
  // (w^p)^n -> w^(p*n)
  if (v->isPow()) {
    TreeRef p = v->child(1);
    assert(p->nextTree() == static_cast<Tree*>(n));
    // PowU PowV w p n
    v->removeNode();
    MoveNodeAtNode(p, SharedTreeStack->push<Type::Mult>(2));
    // PowU w Mult<2> p n
    ShallowSystematicReduce(p);
    ShallowSystematicReduce(u);
    return true;
  }
  // (w1*...*wk)^n -> w1^n * ... * wk^n
  if (v->isMult()) {
    for (Tree* w : v->children()) {
      TreeRef m = SharedTreeStack->push(Type::Pow);
      w->clone();
      n->clone();
      w->moveTreeOverTree(m);
      ShallowSystematicReduce(m);
    }
    n->removeTree();
    u->removeNode();
    ShallowSystematicReduce(u);
    return true;
  }
  // exp(a)^b -> exp(a*b)
  return PatternMatching::MatchReplaceSimplify(u, KPow(KExp(KA), KB),
                                               KExp(KMult(KA, KB)));
}

void Simplification::ConvertPowerRealToPower(Tree* u) {
  u->cloneNodeOverNode(KPow);
  ShallowSystematicReduce(u);
}

bool Simplification::SimplifyPowerReal(Tree* u) {
  assert(u->isPowReal());
  /* Return :
   * - x^y if x is complex or positive or y is integer
   * - PowerReal(x,y) if y is not a rational
   * - Looking at y's reduced rational form p/q :
   *   * PowerReal(x,y) if x is of unknown sign and p odd
   *   * Nonreal if q is even and x negative
   *   * |x|^y if p is even
   *   * -|x|^y if p is odd
   */
  Tree* x = u->child(0);
  Tree* y = x->nextTree();
  ComplexSign xSign = ComplexSign::Get(x);
  ComplexSign ySign = ComplexSign::Get(y);
  if (!ySign.canBeNonInteger() ||
      (xSign.isReal() && xSign.realSign().isPositive())) {
    ConvertPowerRealToPower(u);
    return true;
  }

  if (!y->isRational()) {
    // We don't know enough to simplify further.
    return false;
  }

  bool pIsEven = Rational::Numerator(y).isEven();
  bool qIsEven = Rational::Denominator(y).isEven();
  // y is simplified, both p and q can't be even
  assert(!qIsEven || !pIsEven);

  bool xNegative = xSign.realSign().isStrictlyNegative();

  if (!pIsEven && !xNegative) {
    // We don't know enough to simplify further.
    return false;
  }
  assert(xNegative || pIsEven);

  if (xNegative && qIsEven) {
    u->cloneTreeOverTree(KNonReal);
    return true;
  }

  // We can fallback to |x|^y
  x->cloneNodeAtNode(KAbs);
  ShallowSystematicReduce(x);
  ConvertPowerRealToPower(u);

  if (xNegative && !pIsEven) {
    // -|x|^y
    u->cloneTreeAtNode(KMult(-1_e));
    NAry::SetNumberOfChildren(u, 2);
    ShallowSystematicReduce(u);
  }
  return true;
}

bool Simplification::SimplifyLnReal(Tree* u) {
  assert(u->isLnReal());
  // Under real mode, inputted ln(x) must return nonreal if x < 0
  ComplexSign childSign = ComplexSign::Get(u->child(0));
  if (childSign.realSign().isStrictlyNegative() ||
      !childSign.imagSign().canBeNull()) {
    // Child can't be real, positive or null
    u->cloneTreeOverTree(KNonReal);
    return true;
  }
  if (childSign.realSign().canBeStriclyNegative() ||
      !childSign.imagSign().isZero()) {
    // Child can be nonreal or negative, add a dependency in case.
    u->moveTreeOverTree(PatternMatching::Create(
        KDep(KLn(KA), KSet(KLnReal(KA))), {.KA = u->child(0)}));
    u = u->child(0);
  } else {
    // Safely fallback to complex logarithm.
    u->cloneNodeOverNode(KLn);
  }
  ShallowSystematicReduce(u);
  return true;
}

bool Simplification::SimplifyComplexArgument(Tree* tree) {
  assert(tree->isArg());
  const Tree* child = tree->child(0);
  ComplexSign childSign = ComplexSign::Get(child);
  // arg(x + iy) = atan2(y, x)
  Sign realSign = childSign.realSign();
  if (!realSign.isKnown()) {
    return false;
  }
  // TODO: Maybe move this in advanced reduction
  Sign imagSign = childSign.imagSign();
  if (realSign.isZero() && imagSign.isKnown()) {
    if (imagSign.isZero()) {
      // atan2(0, 0) = undef
      tree->cloneTreeOverTree(KOutOfDefinition);
      return true;
    }
    // atan2(y, 0) = π/2 if y > 0, -π/2 if y < 0
    tree->cloneTreeOverTree(imagSign.isStrictlyPositive()
                                ? KMult(1_e / 2_e, π_e)
                                : KMult(-1_e / 2_e, π_e));
  } else if (realSign.isStrictlyPositive() || imagSign.isPositive() ||
             imagSign.isStrictlyNegative()) {
    /* atan2(y, x) = arctan(y/x)      if x > 0
     *               arctan(y/x) + π  if y >= 0 and x < 0
     *               arctan(y/x) - π  if y < 0  and x < 0 */
    tree->moveTreeOverTree(PatternMatching::CreateSimplify(
        KAdd(KATanRad(KMult(KIm(KA), KPow(KRe(KA), -1_e))), KMult(KB, π_e)),
        {.KA = child,
         .KB = realSign.isStrictlyPositive()
                   ? 0_e
                   : (imagSign.isPositive() ? 1_e : -1_e)}));
  } else {
    return false;
  }
  return true;
}

bool Simplification::SimplifyComplexPart(Tree* tree) {
  assert(tree->isRe() || tree->isIm());
  bool isRe = tree->isRe();
  Tree* child = tree->child(0);
  ComplexSign childSign = ComplexSign::Get(child);
  if (childSign.isPure()) {
    if (isRe != childSign.isReal()) {
      // re(x) = 0 or im(x) = 0
      tree->cloneTreeOverTree(0_e);
    } else if (isRe) {
      // re(x) = x
      tree->removeNode();
    } else {
      // im(x) = -i*x
      tree->moveTreeOverTree(
          PatternMatching::CreateSimplify(KMult(-1_e, i_e, KA), {.KA = child}));
    }
    return true;
  }
  if (!child->isAdd()) {
    return false;
  }
  // Extract pure real or pure imaginary in addition
  TreeRef a(SharedTreeStack->push<Type::Add>(0));
  const int nbChildren = child->numberOfChildren();
  int nbChildrenRemoved = 0;
  int nbChildrenOut = 0;
  Tree* elem = child->firstChild();
  for (int i = 0; i < nbChildren; i++) {
    ComplexSign elemSign = ComplexSign::Get(elem);
    if (elemSign.isPure()) {
      if (isRe != elemSign.isReal()) {
        // re(x) = 0 or im(x) = 0
        elem->removeTree();
      } else if (isRe) {
        // re(x) = x
        elem->detachTree();
        nbChildrenOut++;
      } else {
        // im(x) = -i*x
        TreeRef t(SharedTreeStack->push<Type::Mult>(3));
        (-1_e)->clone();
        (i_e)->clone();
        elem->detachTree();
        ShallowSystematicReduce(t);
        nbChildrenOut++;
      }
      nbChildrenRemoved++;
    } else {
      elem = elem->nextTree();
    }
  }
  NAry::SetNumberOfChildren(child, nbChildren - nbChildrenRemoved);
  NAry::SetNumberOfChildren(a, nbChildrenOut);
  if (nbChildrenOut == 0) {
    a->removeTree();
    return nbChildrenRemoved > 0;
  }
  bool includeOriginalTree = true;
  if (child->numberOfChildren() == 0) {
    // Remove re/im(add) tree
    tree->removeTree();
    includeOriginalTree = false;
  } else if (child->numberOfChildren() == 1) {
    // Shallow reduce to remove the Add node
    Simplification::ShallowSystematicReduce(child);
  }
  tree->moveTreeBeforeNode(a);
  // Increase the number of children of a to include the original re/im
  NAry::SetNumberOfChildren(tree, nbChildrenOut + includeOriginalTree);
  // Shallow reduce new tree
  ShallowSystematicReduce(tree);
  return true;
}

bool Simplification::SimplifySign(Tree* expr) {
  assert(expr->isSign());
  ComplexSign sign = ComplexSign::Get(expr->firstChild());
  const Tree* result;
  if (sign.isZero()) {
    result = 0_e;
  } else if (!sign.isReal()) {
    // Could use sign(z) = exp(i*arg(z)) but sign(z) is preferred. Advanced ?
    return false;
  } else if (sign.realSign().isStrictlyPositive()) {
    result = 1_e;
  } else if (sign.realSign().isStrictlyNegative()) {
    result = -1_e;
  } else {
    return false;
  }
  expr->cloneTreeOverTree(result);
  return true;
}

bool Simplification::SimplifyDistribution(Tree* expr) {
  const Tree* child = expr->child(0);
  const Tree* abscissae[DistributionMethod::k_maxNumberOfParameters];
  DistributionMethod::Type methodType = DistributionMethod::Get(expr);
  for (int i = 0; i < DistributionMethod::numberOfParameters(methodType); i++) {
    abscissae[i] = child;
    child = child->nextTree();
  }
  Distribution::Type distributionType = Distribution::Get(expr);
  const Tree* parameters[Distribution::k_maxNumberOfParameters];
  for (int i = 0; i < Distribution::numberOfParameters(distributionType); i++) {
    parameters[i] = child;
    child = child->nextTree();
  }
  const DistributionMethod* method = DistributionMethod::Get(methodType);
  const Distribution* distribution = Distribution::Get(distributionType);
  bool parametersAreOk;
  bool couldCheckParameters =
      distribution->expressionParametersAreOK(&parametersAreOk, parameters);
  if (!couldCheckParameters) {
    return false;
  }
  if (!parametersAreOk) {
    expr->cloneTreeOverTree(KOutOfDefinition);
    return true;
  }
  return method->shallowReduce(abscissae, distribution, parameters, expr);
}

bool ShouldApproximateOnSimplify(Dimension dimension) {
  // Only angle units are expected not to be approximated.
  return (dimension.isUnit() && !dimension.isAngleUnit());
}

bool RelaxProjectionContext(void* context) {
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);
  if (projectionContext->m_strategy == Strategy::ApproximateToFloat) {
    // Nothing more can be done.
    return false;
  }
  projectionContext->m_strategy = Strategy::ApproximateToFloat;
  return true;
}

bool Simplification::SimplifyWithAdaptiveStrategy(
    Tree* e, ProjectionContext* projectionContext) {
  assert(projectionContext);
  // Clone the tree, and use an adaptive strategy to handle pool overflow.
  SharedTreeStack->executeAndReplaceTree(
      [](void* context, const void* data) {
        Tree* e = static_cast<const Tree*>(data)->clone();
        if (e->isStore()) {
          // Store is an expression only for convenience
          e = e->child(0);
        }
        // Copy ProjectionContext to avoid altering the original
        ProjectionContext projectionContext =
            *static_cast<ProjectionContext*>(context);
        ToSystem(e, &projectionContext);
        SimplifySystem(e, true);
        // TODO: Should be in SimplifySystem but projectionContext is needed.
        TryApproximationStrategyAgain(e, projectionContext);
        Beautification::DeepBeautify(e, projectionContext);
      },
      projectionContext, e, RelaxProjectionContext);
  /* TODO: Due to projection/beautification cycles and multiple intermediary
   * steps, keeping track of a changed status is unreliable. We could compare
   * CRC instead. */
  return true;
}

bool Simplification::PrepareForProjection(
    Tree* e, ProjectionContext* projectionContext) {
  // Seed random nodes before anything is merged/duplicated.
  int maxRandomSeed = Random::SeedRandomNodes(e, 0);
  bool changed = maxRandomSeed > 0;
  // Replace functions and variable before dimension check
  changed = Variables::ProjectLocalVariablesToId(e) || changed;
  // Replace local variables before user named
  if (Projection::DeepReplaceUserNamed(e, *projectionContext)) {
    // Seed random nodes that may have appeared after replacing.
    maxRandomSeed = Random::SeedRandomNodes(e, maxRandomSeed);
    changed = true;
  }
  if (!Dimension::DeepCheckDimensions(e) ||
      !Dimension::DeepCheckListLength(e)) {
    e->cloneTreeOverTree(KUndefUnhandledDimension);
    changed = true;
  }
  return changed;
}

bool Simplification::ToSystem(Tree* e, ProjectionContext* projectionContext) {
  /* 1 - Prepare for projection */
  bool changed = PrepareForProjection(e, projectionContext);
  /* 2 - Extract units */
  if (e->isUnitConversion()) {
    // TODO_PCJ actually extract required unit for later beautification.
    e->moveTreeOverTree(e->child(0));
    changed = true;
  }
  projectionContext->m_dimension = Dimension::GetDimension(e);
  if (ShouldApproximateOnSimplify(projectionContext->m_dimension)) {
    projectionContext->m_strategy = Strategy::ApproximateToFloat;
  }
  if (projectionContext->m_dimension.hasNonKelvinTemperatureUnit()) {
    // Convert the expression to Kelvin at root level.
    Units::Unit::RemoveTemperatureUnit(e);
    changed = true;
  }
  return Projection::DeepSystemProject(e, *projectionContext) || changed;
}

bool Simplification::SimplifySystem(Tree* e, bool advanced) {
  bool changed = DeepSystematicReduce(e);
  changed = List::BubbleUp(e, ShallowSystematicReduce) || changed;
  if (advanced) {
    changed = AdvancedSimplification::AdvancedReduce(e) || changed;
  }
  return Dependency::DeepRemoveUselessDependencies(e) || changed;
}

bool Simplification::TryApproximationStrategyAgain(
    Tree* e, ProjectionContext projectionContext) {
  // Approximate again in case exact numbers appeared during simplification.
  if (projectionContext.m_strategy != Strategy::ApproximateToFloat ||
      !Approximation::ApproximateAndReplaceEveryScalar(e)) {
    return false;
  }
  // NAries could be sorted again, some children may be merged.
  DeepSystematicReduce(e);
  return true;
}

}  // namespace Poincare::Internal
