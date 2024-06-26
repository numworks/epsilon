#include "systematic_reduction.h"

#include <poincare/src/memory/n_ary.h>

#include "arithmetic.h"
#include "binary.h"
#include "dependency.h"
#include "derivation.h"
#include "list.h"
#include "logarithm.h"
#include "matrix.h"
#include "systematic_operation.h"
#include "trigonometry.h"
#include "undefined.h"

namespace Poincare::Internal {

bool SystematicReduction::DeepReduce(Tree* u) {
  /* Although they are also flattened in ShallowReduce, flattening
   * here could save multiple ShallowReduce and flatten calls. */
  bool modified = (u->isMult() || u->isAdd()) && NAry::Flatten(u);
  // Never simplify any dependencies
  if (!u->isSet()) {
    for (Tree* child : u->children()) {
      modified |= DeepReduce(child);
    }
  }

#if ASSERTIONS
  TreeRef previousTree = u->cloneTree();
#endif
  bool shallowModified = ShallowReduce(u);
#if ASSERTIONS
  assert(shallowModified != u->treeIsIdenticalTo(previousTree));
  previousTree->removeTree();
#endif
  return shallowModified || modified;
}

bool SystematicReduction::ShallowReduce(Tree* u) {
  bool changed = BubbleUpFromChildren(u);
  return SimplifySwitch(u) || changed;
}

bool SystematicReduction::BubbleUpFromChildren(Tree* u) {
  /* Before systematic reduction, look for things to bubble-up in children. At
   * this step, only children have been shallowReduced. By doing this before
   * shallowReduction, we don't have to handle undef, float and dependency
   * children in specialized systematic reduction. */
  bool bubbleUpFloat = false, bubbleUpDependency = false, bubbleUpUndef = false;
  for (const Tree* child : u->children()) {
    if (child->isFloat()) {
      bubbleUpFloat = true;
    } else if (child->isDependency()) {
      bubbleUpDependency = true;
    } else if (child->isUndefined()) {
      bubbleUpUndef = true;
    }
  }

  if (bubbleUpUndef && Undefined::ShallowBubbleUpUndef(u)) {
    ShallowReduce(u);
    return true;
  }

  if (bubbleUpFloat && Approximation::ApproximateAndReplaceEveryScalar(u)) {
    ShallowReduce(u);
    return true;
  }

  if (bubbleUpDependency && Dependency::ShallowBubbleUpDependencies(u)) {
    assert(u->isDependency());
    /* u->child(0) may now be reduced again. This could unlock further
     * simplifications. */
    ShallowReduce(u->child(0)) && ShallowReduce(u);
    return true;
  }

  return false;
}

bool SystematicReduction::SimplifySwitch(Tree* u) {
  // This assert is quite costly, should be an assert level 2 ?
  assert(Dimension::DeepCheckDimensions(u));
  if (!u->isNAry() && u->numberOfChildren() == 0) {
    // No childless tree have a reduction pattern.
    return false;
  }
  switch (u->type()) {
    case Type::Abs:
      return SystematicOperation::ReduceAbs(u);
    case Type::Add:
      return SystematicOperation::ReduceAddition(u);
    case Type::ATanRad:
      return Trigonometry::SimplifyArcTangentRad(u);
    case Type::ATrig:
      return Trigonometry::SimplifyATrig(u);
    case Type::Binomial:
      return Arithmetic::SimplifyBinomial(u);
    case Type::Arg:
      return SystematicOperation::ReduceComplexArgument(u);
    case Type::NthDiff:
      return Derivation::ShallowSimplify(u);
    case Type::Dim:
      return SystematicOperation::ReduceDim(u);
    case Type::Distribution:
      return SystematicOperation::ReduceDistribution(u);
    case Type::Exp:
      return SystematicOperation::ReduceExp(u);
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
      return SystematicOperation::ReduceComplexPart(u);
    case Type::LCM:
      return Arithmetic::SimplifyLCM(u);
    case Type::ListSort:
    case Type::Median:
      return List::ShallowApplyListOperators(u);
    case Type::Ln:
      return Logarithm::SimplifyLn(u);
    case Type::LnReal:
      return SystematicOperation::ReduceLnReal(u);
    case Type::Mult:
      return SystematicOperation::ReduceMultiplication(u);
    case Type::Permute:
      return Arithmetic::SimplifyPermute(u);
    case Type::Piecewise:
      return Binary::SimplifyPiecewise(u);
    case Type::Pow:
      return SystematicOperation::ReducePower(u);
    case Type::PowReal:
      return SystematicOperation::ReducePowerReal(u);
    case Type::Quo:
    case Type::Rem:
      return Arithmetic::SimplifyQuotientOrRemainder(u);
    case Type::Round:
      return Arithmetic::SimplifyRound(u);
    case Type::Sign:
      return SystematicOperation::ReduceSign(u);
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

}  // namespace Poincare::Internal
