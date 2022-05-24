#include <poincare/addition.h>
#include <poincare/code_point_layout.h>
#include <poincare/division.h>
#include <poincare/horizontal_layout.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/percent.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/subtraction.h>
#include <poincare/rational.h>

namespace Poincare {

// Property

bool PercentNode::childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const {
  if (child.type() == Type::Conjugate) {
    return childAtIndexNeedsUserParentheses(child.childAtIndex(0), childIndex);
  }
  if (childIndex == 1) {
    // Parentheses are needed to avoid confusion with percent's first child.
    return child.isOfType({Type::Subtraction, Type::Addition});
  }
  return false;
}

// Layout

bool PercentNode::childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const {
  /*  2
   * --- % ---> [2/3]%
   *  3
   */
  if (static_cast<const ExpressionNode *>(child)->type() == Type::Rational && !static_cast<const RationalNode *>(child)->isInteger()) {
    return true;
  }
  return static_cast<const ExpressionNode *>(child)->isOfType({Type::Division, Type::Power});
}

// Simplification

Expression PercentNode::shallowReduce(ReductionContext reductionContext) {
  return Percent(this).shallowReduce(reductionContext);
}

Expression PercentNode::shallowBeautify(ReductionContext * reductionContext) {
  return Percent(this).shallowBeautify(reductionContext);
}

Expression PercentNode::deepBeautify(ReductionContext reductionContext) {
  return Percent(this).deepBeautify(reductionContext);
}

Layout PercentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(numberOfChildren() == 1 || numberOfChildren() == 2);
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  int childrenCount = result.numberOfChildren();
  if (numberOfChildren() == 2) {
    // If second element is an Opposite, there is already the - operator
    if (childAtIndex(1)->type() != ExpressionNode::Type::Opposite) {
      result.addChildAtIndex(CodePointLayout::Builder('+'), childrenCount, childrenCount, nullptr);
      childrenCount++;
    }
    result.addOrMergeChildAtIndex(childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), childrenCount, false);
    childrenCount = result.numberOfChildren();
  }
  result.addChildAtIndex(CodePointLayout::Builder('%'), childrenCount, childrenCount, nullptr);
  return std::move(result);
}

int PercentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(numberOfChildren() == 1 || numberOfChildren() == 2);
  int numberOfChar = SerializationHelper::SerializeChild(childAtIndex(0), this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  if ((numberOfChar < 0) || (numberOfChar >= bufferSize-1)) {
    return numberOfChar;
  }
  if (numberOfChildren() == 2) {
    // If second element is an Opposite, there is already the - operator
    if (childAtIndex(1)->type() != ExpressionNode::Type::Opposite) {
      numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize-numberOfChar, '+');
      if ((numberOfChar < 0) || (numberOfChar >= bufferSize-1)) {
        return numberOfChar;
      }
    }
    numberOfChar += SerializationHelper::SerializeChild(childAtIndex(1), this, buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
    if ((numberOfChar < 0) || (numberOfChar >= bufferSize-1)) {
      return numberOfChar;
    }
  }
  numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize-numberOfChar, '%');
  return numberOfChar;
}

template <typename T> Evaluation<T> PercentNode::templateApproximate(ApproximationContext approximationContext, bool * inputIsUndefined) const {
  if (numberOfChildren() == 1) {
    return Complex<T>::Builder(childAtIndex(0)->approximate(T(), approximationContext).toScalar() / 100.0);
  }
  assert(numberOfChildren() == 2);
  Evaluation<T> aInput = childAtIndex(0)->approximate(T(), approximationContext);
  Evaluation<T> bInput = childAtIndex(1)->approximate(T(), approximationContext);
  T a = aInput.toScalar();
  T b = bInput.toScalar();
  return Complex<T>::Builder(a * (1.0 + b/100.0));
}

Expression Percent::ParseTarget(Expression & leftHandSide) {
  assert(!leftHandSide.isUninitialized());
  /* We must extract the parameters for the percent operation :
   * 1+2+3% -> Percent(1+2,3)
   * 1+2-3% -> Percent(1+2,-3)
   * 1-3%   -> Percent(1,-3)
   * 2*3%   -> Percent(2*3)
   */
  Expression rightHandSide;
  if (leftHandSide.type() == ExpressionNode::Type::Addition) {
    // Extract last addition child into second parameter
    int lastChildIndex = leftHandSide.numberOfChildren() - 1;
    rightHandSide = leftHandSide.childAtIndex(lastChildIndex);
    static_cast<Addition &>(leftHandSide).removeChildAtIndexInPlace(lastChildIndex);
    if (rightHandSide.type() == ExpressionNode::Type::Subtraction) {
      /* The extracted parameter was a subtraction. Its first element must be
       * set back into the addition. */
      static_cast<Addition &>(leftHandSide).addChildAtIndexInPlace(rightHandSide.childAtIndex(0), lastChildIndex, lastChildIndex);
      rightHandSide = Opposite::Builder(rightHandSide.childAtIndex(1));
    }
    if (leftHandSide.numberOfChildren() == 1) {
      // Remove single addition
      leftHandSide = leftHandSide.childAtIndex(0);
    }
  } else if (leftHandSide.type() == ExpressionNode::Type::Subtraction) {
    // Subtraction elements must be separated
    rightHandSide = Opposite::Builder(leftHandSide.childAtIndex(1));
    leftHandSide = leftHandSide.childAtIndex(0);
  } else {
    // Percent apply on only on element
    if (leftHandSide.type() != ExpressionNode::Type::Multiplication) {
      return Percent::Builder({leftHandSide});
    }
    // a*b*c% -> a*b*(c%)
    int lastIndex = leftHandSide.numberOfChildren()-1;
    leftHandSide.replaceChildAtIndexInPlace(lastIndex, Percent::Builder({leftHandSide.childAtIndex(lastIndex)}));
    return leftHandSide;
  }
  assert(!rightHandSide.isUninitialized());
  return Percent::Builder({leftHandSide, rightHandSide});
}

Expression Percent::shallowBeautify(ExpressionNode::ReductionContext * reductionContext) {
  Expression result;
  // Beautify Percent into what is actually computed
  if (numberOfChildren() == 1) {
    // a% -> a/100
    result = Division::Builder(childAtIndex(0), Rational::Builder(100));
  } else {
    assert(numberOfChildren() == 2);
    Expression ratio;
    Expression positiveArg = childAtIndex(1).makePositiveAnyNegativeNumeralFactor(*reductionContext);
    if (!positiveArg.isUninitialized()) {
      // a-b% -> a*(1-b/100)
      ratio = Subtraction::Builder(Rational::Builder(1), Division::Builder(positiveArg, Rational::Builder(100)));
    } else {
      // a+b% -> a*(1+b/100)
      ratio = Addition::Builder(Rational::Builder(1), Division::Builder(childAtIndex(1), Rational::Builder(100)));
    }
    result = Multiplication::Builder({childAtIndex(0), ratio});
  }
  replaceWithInPlace(result);
  return result;
}

Expression Percent::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return *this;
}

Expression Percent::deepBeautify(ExpressionNode::ReductionContext reductionContext) {
  Expression e = shallowBeautify(&reductionContext);
  if (numberOfChildren() == 1) {
    assert(e.type() == ExpressionNode::Type::Division);
    SimplificationHelper::deepBeautifyChildren(e, reductionContext);
  } else {
    /* Overriding deepBeautify to prevent the shallow reduce of the addition
     * because we need to preserve the order. */
    assert(e.numberOfChildren() == 2);
    Expression child0 = e.childAtIndex(0);
    child0 = child0.deepBeautify(reductionContext);
    // We add missing Parentheses after beautifying the parent and child
    if (e.node()->childAtIndexNeedsUserParentheses(child0, 0)) {
      e.replaceChildAtIndexInPlace(0, Parenthesis::Builder(child0));
    }
    // Skip the Addition's shallowBeautify
    Expression child1 = e.childAtIndex(1);
    assert(child1.type() == ExpressionNode::Type::Addition || child1.type() == ExpressionNode::Type::Subtraction);
    SimplificationHelper::deepBeautifyChildren(child1, reductionContext);
    // We add missing Parentheses after beautifying the parent and child
    if (e.node()->childAtIndexNeedsUserParentheses(child1, 0)) {
      e.replaceChildAtIndexInPlace(1, Parenthesis::Builder(child1));
    }
  }
  return e;
}

}
