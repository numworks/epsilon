#include "systematic_reduction.h"

#include <poincare/src/memory/n_ary.h>

#include "approximation.h"
#include "arithmetic.h"
#include "binary.h"
#include "dependency.h"
#include "derivation.h"
#include "k_tree.h"
#include "list.h"
#include "logarithm.h"
#include "matrix.h"
#include "systematic_operation.h"
#include "trigonometry.h"
#include "undefined.h"

namespace Poincare::Internal {

bool SystematicReduction::DeepReduce(Tree* e) {
  /* Although they are also flattened in ShallowReduce, flattening
   * here could save multiple ShallowReduce and flatten calls. */
  bool modified = (e->isMult() || e->isAdd()) && NAry::Flatten(e);
  // Never simplify any dependencies
  if (!e->isDepList()) {
    for (Tree* child : e->children()) {
      modified |= DeepReduce(child);
    }
  }

#if ASSERTIONS
  TreeRef previousTree = e->cloneTree();
#endif
  bool shallowModified = ShallowReduce(e);
#if ASSERTIONS
  assert(shallowModified != e->treeIsIdenticalTo(previousTree));
  previousTree->removeTree();
#endif
  return shallowModified || modified;
}

bool SystematicReduction::ShallowReduce(Tree* e) {
  bool changed = BubbleUpFromChildren(e);
  return Switch(e) || changed;
}

bool SystematicReduction::BubbleUpFromChildren(Tree* e) {
  /* Before systematic reduction, look for things to bubble-up in children. At
   * this step, only children have been shallowReduced. By doing this before
   * shallowReduction, we don't have to handle undef, float and dependency
   * children in specialized systematic reduction. */
  bool bubbleUpFloat = false, bubbleUpDependency = false, bubbleUpUndef = false;
  for (const Tree* child : e->children()) {
    if (child->isFloat()) {
      bubbleUpFloat = true;
    } else if (child->isDep()) {
      bubbleUpDependency = true;
    } else if (child->isUndefined()) {
      bubbleUpUndef = true;
    }
  }

  if (e->isDep() && Dependency::Main(e)->isNonReal()) {
    /* dep(nonreal,{...}) can be undef, so we can't replace the whole expression
     * with nonreal */
    bubbleUpUndef = false;
  }

  if (bubbleUpUndef && Undefined::ShallowBubbleUpUndef(e)) {
    ShallowReduce(e);
    return true;
  }

  if (bubbleUpFloat && Approximation::ApproximateAndReplaceEveryScalar(e)) {
    ShallowReduce(e);
    return true;
  }

  if (bubbleUpDependency && Dependency::ShallowBubbleUpDependencies(e)) {
    assert(e->isDep());
    /* e->child(0) may now be reduced again. This could unlock further
     * simplifications. */
    ShallowReduce(e->child(0)) && ShallowReduce(e);
    return true;
  }

  return false;
}

bool SystematicReduction::Switch(Tree* e) {
  // This assert is quite costly, should be an assert level 2 ?
  assert(Dimension::DeepCheck(e));
  if (!e->isNAry() && e->numberOfChildren() == 0) {
    // No childless tree have a reduction pattern.
    return false;
  }
  switch (e->type()) {
    case Type::Abs:
      return SystematicOperation::ReduceAbs(e);
    case Type::Add:
    case Type::Mult:
      return SystematicOperation::ReduceAddOrMult(e);
    case Type::ArCosH:
      return Trigonometry::ReduceArCosH(e);
    case Type::ATanRad:
      return Trigonometry::ReduceArcTangentRad(e);
    case Type::ATrig:
      return Trigonometry::ReduceATrig(e);
    case Type::Binomial:
      return Arithmetic::ReduceBinomial(e);
    case Type::Arg:
      return SystematicOperation::ReduceComplexArgument(e);
    case Type::Diff:
      return Derivation::Reduce(e);
    case Type::Dim:
      return SystematicOperation::ReduceDim(e);
    case Type::Distribution:
      return SystematicOperation::ReduceDistribution(e);
    case Type::Exp:
      return SystematicOperation::ReduceExp(e);
    case Type::Fact:
      return Arithmetic::ReduceFactorial(e);
    case Type::Factor:
      return Arithmetic::ReduceFactor(e);
    case Type::Floor:
      return Arithmetic::ReduceFloor(e);
    case Type::GCD:
      return Arithmetic::ReduceGCD(e);
    case Type::Im:
    case Type::Re:
      return SystematicOperation::ReduceComplexPart(e);
    case Type::LCM:
      return Arithmetic::ReduceLCM(e);
    case Type::ListSort:
    case Type::Median:
      return List::ShallowApplyListOperators(e);
    case Type::Ln:
      return Logarithm::ReduceLn(e);
    case Type::Permute:
      return Arithmetic::ReducePermute(e);
    case Type::Piecewise:
      return Binary::ReducePiecewise(e);
    case Type::Pow:
      return SystematicOperation::ReducePower(e);
    case Type::PowReal:
      return SystematicOperation::ReducePowerReal(e);
    case Type::Quo:
    case Type::Rem:
      return Arithmetic::ReduceQuotientOrRemainder(e);
    case Type::Round:
      return Arithmetic::ReduceRound(e);
    case Type::Sign:
      return SystematicOperation::ReduceSign(e);
    case Type::Sum:
    case Type::Product:
      return Parametric::ReduceSumOrProduct(e);
    case Type::Trig:
      return Trigonometry::ReduceTrig(e);
    case Type::TrigDiff:
      return Trigonometry::ReduceTrigDiff(e);
    default:
      if (e->type().isListToScalar()) {
        return List::ShallowApplyListOperators(e);
      }
      if (e->type().isLogicalOperator()) {
        return Binary::ReduceBooleanOperator(e);
      }
      if (e->type().isComparison()) {
        return Binary::ReduceComparison(e);
      }
      if (e->isAMatrixOrContainsMatricesAsChildren()) {
        return Matrix::SystematicReduceMatrixOperation(e);
      }
      return false;
  }
}

// TODO: NonNull(x) should be reduced using ComplexSign::canBeNull()

}  // namespace Poincare::Internal
