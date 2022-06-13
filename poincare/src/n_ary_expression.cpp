#include <poincare/n_ary_expression.h>
#include <poincare/rational.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <utility>

namespace Poincare {

void NAryExpressionNode::sortChildrenInPlace(ExpressionOrder order, Context * context, bool canSwapMatrices) {
  Expression reference(this);
  const int childrenCount = reference.numberOfChildren();
  for (int i = 1; i < childrenCount; i++) {
    bool isSorted = true;
    for (int j = 0; j < childrenCount-1; j++) {
      /* Warning: Matrix operations are not always commutative (ie,
       * multiplication) so we never swap 2 matrices. */
      ExpressionNode * cj = childAtIndex(j);
      ExpressionNode * cj1 = childAtIndex(j+1);
      bool cjIsMatrix = Expression(cj).deepIsMatrix(context);
      bool cj1IsMatrix = Expression(cj1).deepIsMatrix(context);
      bool cj1GreaterThanCj = order(cj, cj1) > 0;
      if ((cjIsMatrix && !cj1IsMatrix) || // we always put matrices at the end of expressions
          (cjIsMatrix && cj1IsMatrix && canSwapMatrices && cj1GreaterThanCj) ||
          (!cjIsMatrix && !cj1IsMatrix && cj1GreaterThanCj)) {
        reference.swapChildrenInPlace(j, j+1);
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
  // Multiplication is associative: a*(b*c)->a*b*c
  // The same goes for Addition
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

int NAryExpression::allChildrenAreReal(Context * context) const {
  int i = 0;
  int result = 1;
  while (i < numberOfChildren()) {
    Expression c = childAtIndex(i);
    if (c.type() == ExpressionNode::Type::ComplexCartesian) {
      result *= 0;
    } else if (!c.isReal(context)) {
      return -1;
    }
    i++;
  }
  return result;
}

Expression NAryExpression::checkChildrenAreRationalIntegersAndUpdate(const ExpressionNode::ReductionContext& reductionContext) {
  for (int i = 0; i < numberOfChildren(); ++i) {
    Expression c = childAtIndex(i);
    if (c.deepIsMatrix(reductionContext.context())) {
      return replaceWithUndefinedInPlace();
    }
    if (c.type() != ExpressionNode::Type::Rational) {
      /* Replace expression with undefined if child can be approximated to a
       * complex or finite non-integer number. Otherwise, rely on template
       * approximations. hasDefinedComplexApproximation is given Cartesian
       * complex format to force imaginary part approximation. */
      if (!c.isReal(reductionContext.context()) && c.hasDefinedComplexApproximation(reductionContext.context(), Preferences::ComplexFormat::Cartesian, reductionContext.angleUnit())) {
        return replaceWithUndefinedInPlace();
      }
      // If c was complex but with a null imaginary part, real part is checked.
      float app = c.approximateToScalar<float>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), true);
      if (std::isfinite(app) && app != std::round(app)) {
        return replaceWithUndefinedInPlace();
      }
      // Note : Child could be replaced with the approximation (if finite) here.
      return *this;
    }
    if (!static_cast<Rational &>(c).isInteger()) {
      return replaceWithUndefinedInPlace();
    }
  }
  return Expression();
}

}
