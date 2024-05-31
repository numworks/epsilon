#include "simplification.h"

#include <poincare/src/memory/n_ary.h>

#include "advanced_simplification.h"
#include "beautification.h"
#include "binary.h"
#include "dependency.h"
#include "derivation.h"
#include "infinity.h"
#include "list.h"
#include "matrix.h"
#include "systematic_operation.h"
#include "undefined.h"
#include "variables.h"

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
      return SystematicOperation::SimplifyAbs(u);
    case Type::Add:
      return SystematicOperation::SimplifyAddition(u);
    case Type::ATanRad:
      return Trigonometry::SimplifyArcTangentRad(u);
    case Type::ATrig:
      return Trigonometry::SimplifyATrig(u);
    case Type::Binomial:
      return Arithmetic::SimplifyBinomial(u);
    case Type::Arg:
      return SystematicOperation::SimplifyComplexArgument(u);
    case Type::NthDiff:
      return Derivation::ShallowSimplify(u);
    case Type::Dim:
      return SystematicOperation::SimplifyDim(u);
    case Type::Distribution:
      return SystematicOperation::SimplifyDistribution(u);
    case Type::Exp:
      return SystematicOperation::SimplifyExp(u);
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
      return SystematicOperation::SimplifyComplexPart(u);
    case Type::LCM:
      return Arithmetic::SimplifyLCM(u);
    case Type::ListSort:
    case Type::Median:
      return List::ShallowApplyListOperators(u);
    case Type::Ln:
      return Logarithm::SimplifyLn(u);
    case Type::LnReal:
      return SystematicOperation::SimplifyLnReal(u);
    case Type::Mult:
      return SystematicOperation::SimplifyMultiplication(u);
    case Type::Permute:
      return Arithmetic::SimplifyPermute(u);
    case Type::Piecewise:
      return Binary::SimplifyPiecewise(u);
    case Type::Pow:
      return SystematicOperation::SimplifyPower(u);
    case Type::PowReal:
      return SystematicOperation::SimplifyPowerReal(u);
    case Type::Quo:
    case Type::Rem:
      return Arithmetic::SimplifyQuotientOrRemainder(u);
    case Type::Round:
      return Arithmetic::SimplifyRound(u);
    case Type::Sign:
      return SystematicOperation::SimplifySign(u);
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
        if (projectionContext.m_dimension.isScalar() &&
            projectionContext.m_complexFormat == ComplexFormat::Polar) {
          TurnToPolarForm(e);
        }
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

bool Simplification::TurnToPolarForm(Tree* e) {
  /* Try to turn a scalar x into abs(x)*e^(i×arg(x))
   * If abs or arg stays unreduced, leave x as it was. */
  Tree* result = SharedTreeStack->push<Type::Mult>(2);
  Tree* abs = SharedTreeStack->push(Type::Abs);
  e->clone();
  bool absReduced = ShallowSystematicReduce(abs);
  SharedTreeStack->push(Type::Exp);
  SharedTreeStack->push<Type::Mult>(2);
  SharedTreeStack->push(Type::ComplexI);
  Tree* arg = SharedTreeStack->push(Type::Arg);
  e->clone();
  bool argReduced = ShallowSystematicReduce(arg);
  /* the multiplication that may be created by arg is not flattened on purpose
   * to keep (π/2)*i as such and not as π*i/2 */
  if (!absReduced || !argReduced) {
    SharedTreeStack->dropBlocksFrom(result);
    return false;
  }
  if (abs->isZero() || arg->isZero()) {
    NAry::RemoveChildAtIndex(result, 1);
  }
  if (abs->isOne()) {
    NAry::RemoveChildAtIndex(result, 0);
  }
  NAry::SquashIfPossible(result);
  e->moveTreeOverTree(result);
  return true;
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
