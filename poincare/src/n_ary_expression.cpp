#include <poincare/n_ary_expression.h>
#include <poincare/rational.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <utility>

namespace Poincare {

void NAryExpressionNode::sortChildrenInPlace(ExpressionOrder order, Context * context, bool canSwapMatrices, bool canBeInterrupted) {
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
      bool cj1GreaterThanCj = order(cj, cj1, canBeInterrupted) > 0;
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

Expression NAryExpression::checkChildrenAreRationalIntegers(Context * context) {
  for (int i = 0; i < numberOfChildren(); ++i) {
    Expression c = childAtIndex(i);
    if (c.deepIsMatrix(context)) {
      return replaceWithUndefinedInPlace();
    }
    if (c.type() != ExpressionNode::Type::Rational) {
      return *this;
    }
    if (!static_cast<Rational &>(c).isInteger()) {
      return replaceWithUndefinedInPlace();
    }
  }
  return Expression();
}

}
