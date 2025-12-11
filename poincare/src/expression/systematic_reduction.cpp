#include "systematic_reduction.h"

#include <omg/unreachable.h>
#include <poincare/src/memory/n_ary.h>

#include "approximation.h"
#include "arithmetic.h"
#include "binary.h"
#include "dependency.h"
#include "derivation.h"
#include "integration.h"
#include "list.h"
#include "logarithm.h"
#include "matrix.h"
#include "systematic_operation.h"
#include "trigonometry.h"
#include "undefined.h"

namespace Poincare::Internal {

bool SystematicReduction::DeepReduce(Tree* e) {
  if (e->isDepList()) {
    // Never reduce any dependencies
    return false;
  }
  if (e->isAngleUnitContext()) {
    assert(e->child(0)->isOfType({Type::Cos, Type::Sin, Type::Tan, Type::ACos,
                                  Type::ASin, Type::ATan}) &&
           e->child(0)->numberOfChildren() == 1);
    /* This can happen during early beautification in
     * ShallowBeautifyAngleFunctions. Skip shallow reduction of the un-projected
     * node to avoid calling approximation without the angle unit context. */
    bool result = DeepReduce(e->child(0)->child(0));
    // Skip ShallowReduce on AngleUnitContext because no change is expected
    assert(!ShallowReduce(e));
    return result;
  }
  /* Although they are also flattened in ShallowReduce, flattening
   * here could save multiple ShallowReduce and flatten calls. */
  bool modified = (e->isMult() || e->isAdd()) && NAry::Flatten(e);
  for (Tree* child : e->children()) {
    modified |= DeepReduce(child);
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
#if ASSERTIONS
  // This assert is quite costly, should be an assert level 2 ?
  assert(Dimension::DeepCheck(e));
  Dimension dimBefore = Dimension::Get(e);
  int lenBefore = Dimension::ListLength(e);
#endif
  bool changed = SystematicReduction::BubbleUpFromChildren(e);
  assert(!(changed && Switch(e)));
  changed = changed || Switch(e);
#if ASSERTIONS
  if (changed) {
    assert(Dimension::DeepCheck(e));
    if (!e->isUndefined()) {
      // TODO decide the output of undef ListSort
      /* TODO This assert should always be valid regardless of Undefined state
       * if we output a list of undef/matrix of undef etc... */
      assert(Dimension::ListLength(e) == lenBefore);
      /* Dimension is lost in empty lists.
       * E.g. {}=1 (boolean dimension) is reduced to {} (scalar dimension) */
      assert(lenBefore == 0 || Dimension::Get(e) == dimBefore);
    }
  }
#endif
  return changed;
}

bool SystematicReduction::BubbleUpFromChildren(Tree* e) {
  /* Before systematic reduction, look for things to bubble-up in children. At
   * this step, only children have been shallowReduced. By doing this before
   * shallowReduction, we don't have to handle undef, float and dependency
   * children in specialized systematic reduction. */
  if (e->isDepList()) {
    // Do not bubble-up to preserve well-formed dependencies
    return false;
  }
  if (e->isDep()) {
    /* Special case to handle bubbling up undef directly from dependency list to
     * outer expression. The expression is temporarily malformed after bubbling
     * up from dependency list. */
    bool changed = Undefined::ShallowBubbleUpUndef(Dependency::Dependencies(e));
    if (changed) {
      if (Undefined::ShallowBubbleUpUndef(e)) {
        assert(!ShallowReduce(e));
        return true;
      }
      OMG::unreachable();
    }
  }
  bool bubbleUpFloat = false, bubbleUpDependency = false, bubbleUpUndef = false;
  bool bubbleUpList = TypeBlock::ProducesList(e->type());
  for (const Tree* child : e->children()) {
    if (child->isFloat()) {
      bubbleUpFloat = true;
    } else if (child->isDep()) {
      bubbleUpDependency = true;
    } else if (child->isUndefined()) {
      bubbleUpUndef = true;
    } else if (!bubbleUpList && TypeBlock::ProducesList(child->type())) {
      bubbleUpList = true;
    }
  }

  if (e->isDep() && Dependency::Main(e)->isNonReal()) {
    /* dep(nonreal,{...}) can be undef, so we can't replace the whole expression
     * with nonreal */
    bubbleUpUndef = false;
  }

  if (bubbleUpList && List::BubbleUp(e, ShallowReduce)) {
    assert(!ShallowReduce(e));
    return true;
  }

  if (bubbleUpUndef && Undefined::ShallowBubbleUpUndef(e)) {
    assert(!ShallowReduce(e));
    return true;
  }

  if (bubbleUpFloat &&
      Approximation::ApproximateAndReplaceEveryScalar<double>(e)) {
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
    case Type::Integral:
      return Integration::Reduce(e);
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
    case Type::EuclideanDivision:
      return Arithmetic::ReduceEuclideanDivision(e);
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

// TODO: NonNull(x) should be reduced using ComplexProperties::canBeNull()

}  // namespace Poincare::Internal
