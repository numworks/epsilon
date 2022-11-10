#include "addition.h"
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/src/memory/node_iterator.h>

namespace Poincare {

#if 0
EExpression Addition::shallowReduce(EExpressionNode::ReductionContext reductionContext) {
  {
    EExpression e = SimplificationHelper::defaultShallowReduce(*this, &reductionContext, SimplificationHelper::BooleanReduction::UndefinedOnBooleans);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  // Step 0: Let's remove the addition if it has a single child
  assert(numberOfChildren() > 0);
  if (numberOfChildren() == 1) {
    return squashUnaryHierarchyInPlace();
  }

  /* Step 1: Addition is associative, so let's start by merging children which
   * are additions.
   * If the parent is also an addition, escape and let the parent handle the
   * reduction.
   * Doing this makes it so we reduce the whole addition at once which avoid
   * some reduction errors.
   * (These errors where namely due to child addition being factorized on same
   * denominator before being reduced with the other terms of parent addition.)
   * */
  mergeSameTypeChildrenInPlace();
  EExpression parentOfThis = parent();
  while (!parentOfThis.isUninitialized() && parentOfThis.type() == EExpressionNode::Type::Parenthesis) {
    parentOfThis = parentOfThis.parent();
  }
  if (!parentOfThis.isUninitialized() && (parentOfThis.type() == EExpressionNode::Type::Addition || parentOfThis.type() == EExpressionNode::Type::Subtraction)) {
    return *this;
  }

  const int childrenCount = numberOfChildren();
  assert(childrenCount > 1);

  // Step 2: Sort the children
  sortChildrenInPlace([](const EExpressionNode * e1, const EExpressionNode * e2) { return EExpressionNode::SimplificationOrder(e1, e2, true); }, reductionContext.context(), reductionContext.shouldCheckMatrices());

 // Step 3 : Distribute the addition over lists
  EExpression distributed = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
  if (!distributed.isUninitialized()) {
    return distributed;
  }

  /* Step 4: Handle the units. All children should have the same unit, otherwise
   * the result is not homogeneous. */
  {
    EExpression unit;
    if (childAtIndex(0).removeUnit(&unit).isUndefined()) {
      return replaceWithUndefinedInPlace();
    }
    const bool hasUnit = !unit.isUninitialized();
    for (int i = 1; i < childrenCount; i++) {
      EExpression otherUnit;
      EExpression childI = childAtIndex(i).removeUnit(&otherUnit);
      if (childI.isUndefined()
          || hasUnit == otherUnit.isUninitialized()
          || (hasUnit && !unit.isIdenticalTo(otherUnit)))
      {
        return replaceWithUndefinedInPlace();
      }
    }
    if (hasUnit) {
      /* The Tree is now free of units
       * Recurse to run the reduction, then create the result
       * result = MUL( addition, unit1, unit2...) */
      EExpression addition = shallowReduce(reductionContext);
      assert((addition.type() != EExpressionNode::Type::Nonreal && addition.type() != EExpressionNode::Type::Undefined));
      Multiplication result = Multiplication::Builder(unit);
      // In case `unit` was a multiplication of units, flatten
      result.mergeSameTypeChildrenInPlace();
      addition.replaceWithInPlace(result);
      result.addChildAtIndexInPlace(addition, 0, 1);
      return std::move(result);
    }
  }

  /* Step 5: Handle matrices. We return undef for a scalar added to a matrix.
   * Thanks to the simplification order, all matrix children (if any) are the
   * last children. */
  {
    EExpression lastChild = childAtIndex(childrenCount - 1);
    if (lastChild.deepIsMatrix(reductionContext.context(), reductionContext.shouldCheckMatrices())) {
      if (!childAtIndex(0).deepIsMatrix(reductionContext.context(), reductionContext.shouldCheckMatrices())) {
        /* If there is a matrix in the children, the last child is a matrix. If
         * there is a a scalar, the first child is a scalar. We forbid the
         * addition of a matrix and a scalar. */
        return replaceWithUndefinedInPlace();
      }
      if (lastChild.type() != EExpressionNode::Type::Matrix) {
        /* All children are matrices that are not of type Matrix (for instance a
         * ConfidenceInterval that cannot be reduced). We cannot reduce the
         * addition more. */
        return *this;
      }
      // Create the addition matrix (in place of the last child)
      Matrix resultMatrix = static_cast<Matrix &>(lastChild);
      int n = resultMatrix.numberOfRows();
      int m = resultMatrix.numberOfColumns();
      // Scan to add the other children, which are  matrices
      for (int i = childrenCount - 2; i >= 0; i--) {
        if (childAtIndex(i).type() != EExpressionNode::Type::Matrix) {
          break;
        }
        Matrix currentMatrix = childAtIndex(i).convert<Matrix>();
        int currentN = currentMatrix.numberOfRows();
        int currentM = currentMatrix.numberOfColumns();
        if (currentN != n || currentM != m) {
          // Addition of matrices of different dimensions -> undef
          return replaceWithUndefinedInPlace();
        }
        // Dispatch the current matrix children in the created addition matrix
        for (int j = 0; j < n*m; j++) {
          EExpression resultEntryJ = resultMatrix.childAtIndex(j);
          EExpression currentEntryJ = currentMatrix.childAtIndex(j);
          if (resultEntryJ.type() == EExpressionNode::Type::Addition) {
            static_cast<Addition &>(resultEntryJ).addChildAtIndexInPlace(currentEntryJ, resultEntryJ.numberOfChildren(), resultEntryJ.numberOfChildren());
          } else {
            Addition a = Addition::Builder(resultEntryJ, currentEntryJ);
            resultMatrix.replaceChildAtIndexInPlace(j, a);
          }
        }
        removeChildInPlace(currentMatrix, currentMatrix.numberOfChildren());
      }
      for (int j = 0; j < n*m; j++) {
        resultMatrix.childAtIndex(j).shallowReduce(reductionContext);
      }
      return squashUnaryHierarchyInPlace();
    }
  }

  /* Step 6: Factorize like terms. Thanks to the simplification order, those are
   * next to each other at this point. */
  int i = 0;
  while (i < numberOfChildren()-1) {
    EExpression e1 = childAtIndex(i);
    EExpression e2 = childAtIndex(i+1);
    if (e1.isNumber() && e2.isNumber()) {
      Number r1 = static_cast<Number&>(e1);
      Number r2 = static_cast<Number&>(e2);
      Number a = Number::Addition(r1, r2);
      replaceChildAtIndexInPlace(i, a);
      removeChildAtIndexInPlace(i+1);
      continue;
    }
    if (TermsHaveIdenticalNonNumeralFactors(e1, e2, reductionContext.context())) {
      factorizeChildrenAtIndexesInPlace(i, i+1, reductionContext);
      continue;
    }
    i++;
  }

  // Step 6.2: factorize sin^2+cos^2
  for (int i = 0; i < numberOfChildren(); i++) {
    EExpression baseOfSquaredCos;
    // Find y*cos^2(x)
    if (TermHasSquaredCos(childAtIndex(i), reductionContext, baseOfSquaredCos)) {
      // Try to find y*sin^2(x) and turn sum into y
      EExpression additionWithFactorizedSumOfSquaredTrigFunction = factorizeSquaredTrigFunction(baseOfSquaredCos, reductionContext);
      if (!additionWithFactorizedSumOfSquaredTrigFunction.isUninitialized()) {
        // If it's initialized, it means that the pattern was found
        return additionWithFactorizedSumOfSquaredTrigFunction;
      }
    }
  }

  // Factorizing terms might have created dependencies.
  EExpression eBubbledUp = SimplificationHelper::bubbleUpDependencies(*this, reductionContext);
  if (!eBubbledUp.isUninitialized()) {
    // bubbleUpDependencies shallowReduces the expression
    return eBubbledUp;
  }

  /* Step 7: Let's remove any zero. It's important to do this after having
   * factorized because factorization can lead to new zeroes. For example
   * pi+(-1)*pi. We don't remove the last zero if it's the only child left
   * though. */
  i = 0;
  while (i < numberOfChildren()) {
    EExpression e = childAtIndex(i);
    if (e.type() == EExpressionNode::Type::Rational && static_cast<Rational&>(e).isZero() && numberOfChildren() > 1) {
      removeChildAtIndexInPlace(i);
      continue;
    }
    i++;
  }

  // Step 8: Let's remove the addition altogether if it has a single child
  EExpression result = squashUnaryHierarchyInPlace();
  if (result != *this) {
    return result;
  }

  /* Step 9: Let's bubble up the complex operator if possible
   * 3 cases:
   * - All children are real, we do nothing (allChildrenAreReal == 1)
   * - One of the child is non-real and not a ComplexCartesian: it means a
   *   complex expression could not be resolved as a ComplexCartesian, we cannot
   *   do anything about it now (allChildrenAreReal == -1)
   * - All children are either real or ComplexCartesian (allChildrenAreReal == 0)
   *   We can bubble up ComplexCartesian nodes. */
  if (allChildrenAreReal(reductionContext.context(), reductionContext.shouldCheckMatrices()) == 0) {
    /* We turn (a+ib)+(c+id) into (a+c)+i(c+d)*/
    Addition imag = Addition::Builder(); // we store all imaginary parts in 'imag'
    Addition real = *this; // we store all real parts in 'real'
    i = numberOfChildren() - 1;
    while (i >= 0) {
      EExpression c = childAtIndex(i);
      if (c.type() == EExpressionNode::Type::ComplexCartesian) {
        real.replaceChildAtIndexInPlace(i, c.childAtIndex(0));
        imag.addChildAtIndexInPlace(c.childAtIndex(1), imag.numberOfChildren(), imag.numberOfChildren());
      } else {
        // the Addition is sorted so ComplexCartesian nodes are the last ones
        break;
      }
      i--;
    }
    ComplexCartesian newComplexCartesian = ComplexCartesian::Builder();
    replaceWithInPlace(newComplexCartesian);
    newComplexCartesian.replaceChildAtIndexInPlace(0, real);
    newComplexCartesian.replaceChildAtIndexInPlace(1, imag);
    real.shallowReduce(reductionContext);
    imag.shallowReduce(reductionContext);
    return newComplexCartesian.shallowReduce(reductionContext);
  }

  /* Step 10: Let's put everything under a common denominator.
   * This step is done only for ReductionTarget::User if the parent expression
   * is not an addition. */
  EExpression p = result.parent();
  if (reductionContext.target() == EExpressionNode::ReductionTarget::User && result == *this && (p.isUninitialized() || p.type() != EExpressionNode::Type::Addition)) {
    // squashUnaryHierarchy didn't do anything: we're not an unary hierarchy
     result = factorizeOnCommonDenominator(reductionContext);
  }
  return result;
}
#endif

}
