#include "systematic_reduction.h"

#include <omg/unreachable.h>
#include <poincare/src/memory/n_ary.h>

#include "approximation.h"
#include "arithmetic.h"
#include "binary.h"
#include "dependency.h"
#include "derivation.h"
#include "integration.h"
#include "k_tree.h"
#include "list.h"
#include "logarithm.h"
#include "matrix.h"
#include "systematic_operation.h"
#include "trigonometry.h"
#include "undefined.h"

namespace Poincare::Internal {

/* [isList] explanation:
 * Because we do not want to think of list in SystematicReduction steps, some
 * reductions lose the list dimension when applied to the wrong tree.
 * (e.g.: 0^KA => dep(0, {0^KA})).
 * But checking [Dimenstion::IsList] on each descendant of tree is too costly.
 * Instead we keep track of the presence of lists inside the tree manually.
 * [isList] is set to [true] when a list or a list-producing function (e.g.:
 * RandIntNoRep) is encountered, or when a child is a list and the node is not a
 * [isListToScalar].
 * This allows [ShallowReduceAux] not to call [Switch] when a list is present */

bool SystematicReduction::DeepReduceAux(Tree* e, bool* isList) {
  if (e->isDepList()) {
    // Never simplify any dependencies
    return false;
  }
  bool hasListChild = false;
  /* Although they are also flattened in ShallowReduce, flattening
   * here could save multiple ShallowReduce and flatten calls. */
  bool modified = (e->isMult() || e->isAdd()) && NAry::Flatten(e);
  for (Tree* child : e->children()) {
    bool childIsList = false;
    modified |= DeepReduceAux(child, &childIsList);
    hasListChild = hasListChild || childIsList;
  }
  /* This should be all the nodes capable of producing a list.
   * If a UserFunction is present here, it means there is no definition for it,
   * we assume it does not involve lists */
  if (TypeBlock::ProducesList(e->type())) {
    *isList = true;
  } else if (e->isListToScalar() || e->isDim()) {
    assert(hasListChild || e->isDim());
    *isList = false;
  } else {
    // Bubble-up list
    *isList = hasListChild;
  }

#if ASSERTIONS
  TreeRef previousTree = e->cloneTree();
#endif
  bool shallowModified = ShallowReduceAux(e, *isList);
#if ASSERTIONS
  assert(shallowModified != e->treeIsIdenticalTo(previousTree));
  previousTree->removeTree();
#endif
  return shallowModified || modified;
}

bool SystematicReduction::DeepReduce(Tree* e) {
  bool isList = false;
  return DeepReduceAux(e, &isList);
}

bool SystematicReduction::ShallowReduceAux(Tree* e, bool isList) {
#if ASSERTIONS
  // This assert is quite costly, should be an assert level 2 ?
  assert(Dimension::DeepCheck(e));
  Dimension dimBefore = Dimension::Get(e);
  int lenBefore = Dimension::ListLength(e);
#endif
  bool changed = SystematicReduction::BubbleUpFromChildren(e, isList);
  // NOTE: Use [ShallowReduceMaybeList] if expression may contain lists
  assert(Dimension::IsList(e) == isList);
  /* See comment at the start of file for why we do not call [Switch] when a
   * list is present */
  if (!isList || e->isListSort()) {
    assert(!(changed && Switch(e)));
    changed = changed || Switch(e);
  } else if (e->isMult() || e->isAdd()) {
    /* When an NAry node contains lists, `Switch` is not called and no
     * flatten occurs, doing a `Flatten` here ensures that calling [DeepReduce]
     * again won't change the tree, even when lists are present */
    changed = changed || NAry::Flatten(e);
  }
#if ASSERTIONS
  if (changed) {
    assert(Dimension::DeepCheck(e));
    if (!e->isUndefined()) {
      assert(Dimension::Get(e) == dimBefore);
      // TODO decide the output of undef ListSort
      /* TODO This assert should always be valid regardless of Undefined state
       * if we output a list of undef/matrix of undef etc... */
      assert(Dimension::ListLength(e) == lenBefore);
    }
  }
#endif
  return changed;
}

bool SystematicReduction::ShallowReduce(Tree* e) {
  /* TODO: Here we assumes [e] is not a list as it is too costly to call
   * [Dimension::IsList] on every [ShallowReduce].
   * If the tree contains lists, [ShallowReduceMaybeList] should be used
   * (See comment at start of file) */
  return ShallowReduceMaybeList(e, false);
}

bool SystematicReduction::ShallowReduceMaybeList(Tree* e,
                                                 bool forceDeepListCheck) {
  return ShallowReduceAux(e, forceDeepListCheck
                                 ? Dimension::IsList(e)
                                 : TypeBlock::ProducesList(e->type()));
}

bool SystematicReduction::BubbleUpFromChildren(Tree* e, bool isList) {
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
        assert(!ShallowReduceAux(e, isList));
        return true;
      }
      OMG::unreachable();
    }
  }
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
    assert(!ShallowReduceAux(e, isList));
    return true;
  }

  if (bubbleUpFloat &&
      Approximation::ApproximateAndReplaceEveryScalar<double>(e)) {
    ShallowReduceAux(e, isList);
    return true;
  }

  if (bubbleUpDependency && Dependency::ShallowBubbleUpDependencies(e)) {
    assert(e->isDep());
    /* e->child(0) may now be reduced again. This could unlock further
     * simplifications. */
    ShallowReduceAux(e->child(0), isList) && ShallowReduceAux(e, isList);
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

// TODO: NonNull(x) should be reduced using ComplexSign::canBeNull()

}  // namespace Poincare::Internal
