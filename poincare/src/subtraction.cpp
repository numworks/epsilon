#include <assert.h>
#include <poincare/addition.h>
#include <poincare/layout.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/subtraction.h>

namespace Poincare {

// Private

bool SubtractionNode::childAtIndexNeedsUserParentheses(const OExpression& child,
                                                       int childIndex) const {
  if (childIndex == 0) {
    // First operand of a subtraction never requires parentheses
    return false;
  }
  if (child.isNumber() &&
      static_cast<const Number&>(child).isPositive() == TrinaryBoolean::False) {
    return true;
  }
  if (child.isOfType({Type::Conjugate, Type::Dependency})) {
    return childAtIndexNeedsUserParentheses(child.childAtIndex(0), childIndex);
  }
  return child.isOfType({Type::Subtraction, Type::Opposite, Type::Addition});
}

size_t SubtractionNode::serialize(char* buffer, size_t bufferSize,
                                  Preferences::PrintFloatMode floatDisplayMode,
                                  int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode,
                                    numberOfSignificantDigits, "-");
}

OExpression SubtractionNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Subtraction(this).shallowReduce(reductionContext);
}

OExpression Subtraction::shallowReduce(ReductionContext reductionContext) {
  OExpression e = SimplificationHelper::defaultShallowReduce(
      *this, &reductionContext,
      SimplificationHelper::BooleanReduction::UndefinedOnBooleans);
  if (!e.isUninitialized()) {
    return e;
  }
  OExpression secondChild = childAtIndex(1);
  if (secondChild.otype() == ExpressionNode::Type::Addition) {
    /* In Addition::shallowReduce, the addition is not reduced if its parent is
     * a Subtraction, to avoid miscomputing a common denominator. This results
     * in the second child not being yet reduced. This could cause problem
     * when reducing the multiplication `(-1) * secondChild` since all
     * children of multiplication should be reduced before reducing it. So
     * instead, each child of the addition is multiplied by -1 and reduced.
     * Example: `(A+B)-(C+D)` is reduced into `(A+B)+((-1*C)+(-1*D))`instead of
     * `(A+B)+(-1*(C+D))`
     *  */
    int n = secondChild.numberOfChildren();
    for (int i = 0; i < n; i++) {
      OExpression additionTerm = secondChild.childAtIndex(i);
      Multiplication m = Multiplication::Builder(Rational::Builder(-1));
      secondChild.replaceChildAtIndexInPlace(i, m);
      m.addChildAtIndexInPlace(additionTerm, 1, 1);
      m.shallowReduce(reductionContext);
    }
  } else {
    OExpression m = Multiplication::Builder(Rational::Builder(-1), secondChild);
    replaceChildAtIndexInPlace(1, m);
    m.shallowReduce(reductionContext);
  }
  Addition a = Addition::Builder(childAtIndex(0), childAtIndex(1));
  replaceWithInPlace(a);
  return a.shallowReduce(reductionContext);
}

}  // namespace Poincare
