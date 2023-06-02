#include <poincare/n_ary_expression.h>
#include <poincare/rational.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <utility>

namespace Poincare {

void NAryExpressionNode::sortChildrenInPlace(ExpressionOrder order,
                                             Context* context,
                                             bool canSwapMatrices,
                                             bool canContainMatrices) {
  Expression reference(this);
  const int childrenCount = reference.numberOfChildren();
  for (int i = 1; i < childrenCount; i++) {
    bool isSorted = true;
    for (int j = 0; j < childrenCount - 1; j++) {
      /* Warning: Matrix operations are not always commutative (ie,
       * multiplication) so we never swap 2 matrices. */
      ExpressionNode* cj = childAtIndex(j);
      ExpressionNode* cj1 = childAtIndex(j + 1);
      bool cjIsMatrix =
          Expression(cj).deepIsMatrix(context, canContainMatrices);
      bool cj1IsMatrix =
          Expression(cj1).deepIsMatrix(context, canContainMatrices);
      bool cj1GreaterThanCj = order(cj, cj1) > 0;
      if ((cjIsMatrix &&
           !cj1IsMatrix) ||  // we always put matrices at the end of expressions
          (cjIsMatrix && cj1IsMatrix && canSwapMatrices && cj1GreaterThanCj) ||
          (!cjIsMatrix && !cj1IsMatrix && cj1GreaterThanCj)) {
        reference.swapChildrenInPlace(j, j + 1);
        isSorted = false;
      }
    }
    if (isSorted) {
      return;
    }
  }
}

Expression NAryExpressionNode::squashUnaryHierarchyInPlace() {
  NAryExpression reference = NAryExpression(this);
  if (reference.numberOfChildren() == 1) {
    Expression child = reference.childAtIndex(0);
    reference.replaceWithInPlace(child);
    return child;
  }
  return std::move(reference);
}

void NAryExpression::mergeSameTypeChildrenInPlace() {
  // Multiplication is associative: a*(b*c)->a*b*c. The same goes for Addition
  ExpressionNode::Type parentType = type();
  int i = 0;
  while (i < numberOfChildren()) {
    Expression c = childAtIndex(i);
    if (c.type() != parentType) {
      i++;
    } else {
      mergeChildrenAtIndexInPlace(c, i);
    }
  }
}

bool NAryExpression::allChildrenAreReal(Context* context,
                                        bool canContainMatrices) const {
  int n = numberOfChildren();
  for (int i = 0; i < n; i++) {
    Expression c = childAtIndex(i);
    if (!c.isReal(context, canContainMatrices)) {
      return false;
    }
  }
  return true;
}

Expression NAryExpression::checkChildrenAreRationalIntegersAndUpdate(
    const ReductionContext& reductionContext) {
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; ++i) {
    Expression c = childAtIndex(i);
    if (c.deepIsMatrix(reductionContext.context(),
                       reductionContext.shouldCheckMatrices())) {
      return replaceWithUndefinedInPlace();
    }
    if (c.type() != ExpressionNode::Type::Rational) {
      /* Replace expression with undefined if child can be approximated to a
       * complex or finite non-integer number. Otherwise, rely on template
       * approximations. hasDefinedComplexApproximation is given Cartesian
       * complex format to force imaginary part approximation. */
      if (!c.isReal(reductionContext.context(),
                    reductionContext.shouldCheckMatrices()) &&
          c.hasDefinedComplexApproximation<float>(
              reductionContext.context(), Preferences::ComplexFormat::Cartesian,
              reductionContext.angleUnit())) {
        return replaceWithUndefinedInPlace();
      }
      // If c was complex but with a null imaginary part, real part is checked.
      double app = c.approximateToScalar<double>(
          reductionContext.context(), reductionContext.complexFormat(),
          reductionContext.angleUnit(), true);
      if (std::isfinite(app) && app != std::round(app)) {
        return replaceWithUndefinedInPlace();
      }
      // Note : Child could be replaced with the approximation (if finite) here.
      return *this;
    }
    if (!static_cast<Rational&>(c).isInteger()) {
      return replaceWithUndefinedInPlace();
    }
  }
  return Expression();
}

Expression NAryExpression::combineComplexCartesians(
    ComplexOperator complexOperator, ReductionContext reductionContext) {
  /* Let's bubble up the complex cartesian if possible.
   * Children are sorted so ComplexCartesian nodes are at the end
   */
  int currentNChildren = numberOfChildren();
  if (childAtIndex(currentNChildren - 1).type() !=
      ExpressionNode::Type::ComplexCartesian) {
    return Expression();
  }

  int i = currentNChildren - 1;
  // Merge all ComplexCartesian and real children into one
  ComplexCartesian child = childAtIndex(i).convert<ComplexCartesian>();
  while (i > 0) {
    i--;
    Expression c = childAtIndex(i);
    if (c.type() != ExpressionNode::Type::ComplexCartesian) {
      if (!c.isReal(reductionContext.context(),
                    reductionContext.shouldCheckMatrices())) {
        continue;
      }
      c = ComplexCartesian::Builder(c, Rational::Builder(0));
    }
    assert(c.type() == ExpressionNode::Type::ComplexCartesian);
    child = (child.*complexOperator)(static_cast<ComplexCartesian&>(c),
                                     reductionContext);
    replaceChildAtIndexInPlace(numberOfChildren() - 1, child);
    removeChildAtIndexInPlace(i);
  }
  if (currentNChildren != numberOfChildren()) {
    child.shallowReduce(reductionContext);
    return shallowReduce(reductionContext);
  }

  return Expression();
}

}  // namespace Poincare
