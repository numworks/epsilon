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

template<int T>
bool PercentNode<T>::childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const {
  if (child.type() == Type::Conjugate) {
    return childAtIndexNeedsUserParentheses(child.childAtIndex(0), childIndex);
  }
  if (childIndex == 1) {
    // Parentheses are needed to avoid confusion with percent's first child.
    return child.isOfType({Type::Subtraction, Type::Addition});
  }
  return false;
}

template<>
ExpressionNode::Sign PercentNode<1>::sign(Context * context) const {
  return childAtIndex(0)->sign(context);
}

template<>
ExpressionNode::Sign PercentNode<2>::sign(Context * context) const {
  ExpressionNode::Sign sign0 = childAtIndex(0)->sign(context);
  ExpressionNode::Sign sign1 = childAtIndex(1)->sign(context);
  if (sign0 == sign1) {
    return sign0;
  }
  return Sign::Unknown;
}

template<>
ExpressionNode::NullStatus PercentNode<1>::nullStatus(Context * context) const {
  return childAtIndex(0)->nullStatus(context);
}

template<>
ExpressionNode::NullStatus PercentNode<2>::nullStatus(Context * context) const {
  ExpressionNode::NullStatus nullStatus0 = childAtIndex(0)->nullStatus(context);
  ExpressionNode::NullStatus nullStatus1 = childAtIndex(1)->nullStatus(context);
  if (nullStatus0 != ExpressionNode::NullStatus::NonNull || nullStatus1 == ExpressionNode::NullStatus::Null) {
    return nullStatus0;
  }
  // At this point if the expression has a defined sign, it is a strict sign
  return (sign(context) != ExpressionNode::Sign::Unknown) ? ExpressionNode::NullStatus::NonNull : ExpressionNode::NullStatus::Unknown;
}

// Layout

template<int T>
bool PercentNode<T>::childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const {
  /*  2
   * --- % ---> [2/3]%
   *  3
   */
  return SerializationHelper::PostfixChildNeedsSystemParenthesesAtSerialization(child);
}

// Simplification

template<int T>
Expression PercentNode<T>::shallowReduce(ReductionContext reductionContext) {
  return Percent(this).shallowReduce(reductionContext);
}

template<>
Expression PercentNode<1>::shallowBeautify(ReductionContext * reductionContext) {
  return PercentSimple(this).shallowBeautify(reductionContext);
}

template<>
Expression PercentNode<2>::shallowBeautify(ReductionContext * reductionContext) {
  return PercentAddition(this).shallowBeautify(reductionContext);
}

template<>
Expression PercentNode<1>::deepBeautify(ReductionContext reductionContext) {
  return ExpressionNode::deepBeautify(reductionContext);
}

template<>
Expression PercentNode<2>::deepBeautify(ReductionContext reductionContext) {
  return PercentAddition(this).deepBeautify(reductionContext);
}

template<int T>
Layout PercentNode<T>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(numberOfChildren() == 1 || numberOfChildren() == 2);
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  int childrenCount = result.numberOfChildren();
  if (T == 2) {
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

template<int T>
int PercentNode<T>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  int numberOfChar = SerializationHelper::SerializeChild(childAtIndex(0), this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
  if ((numberOfChar < 0) || (numberOfChar >= bufferSize-1)) {
    return numberOfChar;
  }
  if (T == 2) {
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

template<int T>
template <typename U> Evaluation<U> PercentNode<T>::templateApproximate(ApproximationContext approximationContext, bool * inputIsUndefined) const {
  if (T == 1) {
    return Complex<U>::Builder(childAtIndex(0)->approximate(U(), approximationContext).toScalar() / 100.0);
  }
  Evaluation<U> aInput = childAtIndex(0)->approximate(U(), approximationContext);
  Evaluation<U> bInput = childAtIndex(1)->approximate(U(), approximationContext);
  U a = aInput.toScalar();
  U b = bInput.toScalar();
  return Complex<U>::Builder(a * (1.0 + b/100.0));
}

Expression Percent::ParseTarget(Expression & leftHandSide) {
  assert(!leftHandSide.isUninitialized());
  /* We must extract the parameters for the percent operation :
   * 1+2+3% -> PercentAddition(1+2,3)
   * 1+2-3% -> PercentAddition(1+2,-3)
   * 1-3%   -> PercentAddition(1,-3)
   * 2*3%   -> PercentSimple(2*3)
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
      return PercentSimple::Builder(leftHandSide);
    }
    // a*b*c% -> a*b*(c%)
    int lastIndex = leftHandSide.numberOfChildren()-1;
    leftHandSide.replaceChildAtIndexInPlace(lastIndex, PercentSimple::Builder(leftHandSide.childAtIndex(lastIndex)));
    return leftHandSide;
  }
  assert(!rightHandSide.isUninitialized());
  return PercentAddition::Builder(leftHandSide, rightHandSide);
}

Expression Percent::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  /* Percent Expression is preserved for beautification. Escape cases are
   * therefore not implemented */
  return *this;
}

Expression PercentSimple::shallowBeautify(ExpressionNode::ReductionContext * reductionContext) {
  // Beautify Percent into what is actually computed : a% -> a/100
  Expression result = Division::Builder(childAtIndex(0), Rational::Builder(100));
  replaceWithInPlace(result);
  return result;
}

Expression PercentAddition::shallowBeautify(ExpressionNode::ReductionContext * reductionContext) {
  // Beautify Percent into what is actually computed
  Expression ratio;
  Expression positiveArg = childAtIndex(1).makePositiveAnyNegativeNumeralFactor(*reductionContext);
  if (!positiveArg.isUninitialized()) {
    // a-b% -> a*(1-b/100)
    ratio = Subtraction::Builder(Rational::Builder(1), Division::Builder(positiveArg, Rational::Builder(100)));
  } else {
    // a+b% -> a*(1+b/100)
    ratio = Addition::Builder(Rational::Builder(1), Division::Builder(childAtIndex(1), Rational::Builder(100)));
  }
  Expression result = Multiplication::Builder({childAtIndex(0), ratio});
  replaceWithInPlace(result);
  return result;
}

Expression PercentAddition::deepBeautify(ExpressionNode::ReductionContext reductionContext) {
  Expression e = shallowBeautify(&reductionContext);
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
  return e;
}

template bool PercentNode<1>::childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const;
template bool PercentNode<1>::childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const;
template Expression PercentNode<1>::shallowReduce(ReductionContext reductionContext);
template Layout PercentNode<1>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int PercentNode<1>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template Evaluation<float> PercentNode<1>::templateApproximate<float>(ApproximationContext approximationContext, bool * inputIsUndefined) const;
template Evaluation<double> PercentNode<1>::templateApproximate<double>(ApproximationContext approximationContext, bool * inputIsUndefined) const;

template bool PercentNode<2>::childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const;
template bool PercentNode<2>::childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const;
template Expression PercentNode<2>::shallowReduce(ReductionContext reductionContext);
template Layout PercentNode<2>::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template int PercentNode<2>::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const;
template Evaluation<float> PercentNode<2>::templateApproximate<float>(ApproximationContext approximationContext, bool * inputIsUndefined) const;
template Evaluation<double> PercentNode<2>::templateApproximate<double>(ApproximationContext approximationContext, bool * inputIsUndefined) const;

}
