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
  OExpression reference(this);
  const int childrenCount = reference.numberOfChildren();
  for (int i = 1; i < childrenCount; i++) {
    bool isSorted = true;
    for (int j = 0; j < childrenCount - 1; j++) {
      /* Warning: Matrix operations are not always commutative (ie,
       * multiplication) so we never swap 2 matrices. */
      ExpressionNode* cj = childAtIndex(j);
      ExpressionNode* cj1 = childAtIndex(j + 1);
      bool cjIsMatrix =
          OExpression(cj).deepIsMatrix(context, canContainMatrices);
      bool cj1IsMatrix =
          OExpression(cj1).deepIsMatrix(context, canContainMatrices);
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

OExpression NAryExpressionNode::squashUnaryHierarchyInPlace() {
  NAryExpression reference = NAryExpression(this);
  if (reference.numberOfChildren() == 1) {
    OExpression child = reference.childAtIndex(0);
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
    OExpression c = childAtIndex(i);
    if (c.type() != parentType) {
      i++;
    } else {
      mergeChildrenAtIndexInPlace(c, i);
    }
  }
}

OExpression NAryExpression::checkChildrenAreRationalIntegersAndUpdate(
    const ReductionContext& reductionContext) {
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; ++i) {
    OExpression c = childAtIndex(i);
    if (c.deepIsMatrix(reductionContext.context(),
                       reductionContext.shouldCheckMatrices())) {
      return replaceWithUndefinedInPlace();
    }
    if (c.type() != ExpressionNode::Type::Rational) {
      /* Replace expression with undefined if child can be approximated to a
       * complex or finite non-integer number. Otherwise, rely on template
       * approximations. hasDefinedComplexApproximation is given Cartesian
       * complex format to force imaginary part approximation. */
      ApproximationContext approximationContext(reductionContext, true);
      Preferences::ComplexFormat complexFormat =
          approximationContext.complexFormat();
      approximationContext.setComplextFormat(
          Preferences::ComplexFormat::Cartesian);
      if (!c.isReal(reductionContext.context(),
                    reductionContext.shouldCheckMatrices()) &&
          c.hasDefinedComplexApproximation<float>(approximationContext)) {
        return replaceWithUndefinedInPlace();
      }
      // If c was complex but with a null imaginary part, real part is checked.
      approximationContext.setComplextFormat(complexFormat);
      double app = c.approximateToScalar<double>(approximationContext);
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
  return OExpression();
}

OExpression NAryExpression::combineComplexCartesians(
    ComplexOperator complexOperator, ReductionContext reductionContext) {
  /* Let's bubble up the complex cartesian if possible.
   * Children are sorted so ComplexCartesian nodes are at the end. */
  int currentNChildren = numberOfChildren();
  if (childAtIndex(currentNChildren - 1).type() !=
      ExpressionNode::Type::ComplexCartesian) {
    return OExpression();
  }
  /* We need to shallow reduce with target for analysis otherwise the
   * combination of complex might not be well reduced. */
  ReductionContext contextForAnalysis = reductionContext;
  contextForAnalysis.setTarget(ReductionTarget::SystemForAnalysis);
  int i = currentNChildren - 1;
  // Merge all ComplexCartesian and real children into one
  ComplexCartesian child = childAtIndex(i).convert<ComplexCartesian>();
  child.real().shallowReduce(contextForAnalysis);
  child.imag().shallowReduce(contextForAnalysis);
  while (i > 0) {
    i--;
    OExpression c = childAtIndex(i);
    if (c.type() != ExpressionNode::Type::ComplexCartesian) {
      if (!c.isReal(reductionContext.context(),
                    reductionContext.shouldCheckMatrices())) {
        continue;
      }
      c = ComplexCartesian::Builder(c, Rational::Builder(0));
    }
    assert(c.type() == ExpressionNode::Type::ComplexCartesian);
    ComplexCartesian complex = static_cast<ComplexCartesian&>(c);
    complex.real().shallowReduce(contextForAnalysis);
    complex.imag().shallowReduce(contextForAnalysis);
    child = (child.*complexOperator)(complex, reductionContext);
    replaceChildAtIndexInPlace(numberOfChildren() - 1, child);
    removeChildAtIndexInPlace(i);
  }
  if (currentNChildren != numberOfChildren()) {
    child.shallowReduce(reductionContext);
    return shallowReduce(reductionContext);
  }

  return OExpression();
}

}  // namespace Poincare
