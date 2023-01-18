#include <poincare/opposite.h>
#include <poincare/addition.h>
#include <poincare/code_point_layout.h>
#include <poincare/constant.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <assert.h>
#include <cmath>
#include <stdlib.h>
#include <utility>

namespace Poincare {

int OppositeNode::polynomialDegree(Context * context, const char * symbolName) const {
  return childAtIndex(0)->polynomialDegree(context, symbolName);
}

template<typename T>
Evaluation<T> OppositeNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  Evaluation<T> childEval = childAtIndex(0)->approximate(T(), approximationContext);
  return MultiplicationNode::Compute(Complex<T>::Builder(-1), childEval, approximationContext.complexFormat());
}

/* Layout */

bool OppositeNode::childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const {
  assert(childIndex == 0);
  if (child.isNumber() && static_cast<const Number &>(child).isPositive() == TrinaryBoolean::False) {
    return true;
  }
  if (child.type() == Type::Conjugate) {
    return childAtIndexNeedsUserParentheses(child.childAtIndex(0), 0);
  }
  return child.isOfType({Type::Addition, Type::Subtraction, Type::Opposite});
}

Layout OppositeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  HorizontalLayout result = HorizontalLayout::Builder(CodePointLayout::Builder('-'));
  if (childAtIndex(0)->type() == Type::Opposite || (childAtIndex(0)->type() == Type::Power && childAtIndex(0)->childAtIndex(0)->type() != Type::Symbol)) {
    result.addOrMergeChildAtIndex(LayoutHelper::Parentheses(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context), false), 1, false);
  } else {
    result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context), 1, false);
  }
  return std::move(result);
}

int OppositeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize-1;
  }
  buffer[bufferSize-1] = 0;
  if (bufferSize == 1) {
    return bufferSize-1;
  }
  int numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, '-');
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }
  numberOfChar += childAtIndex(0)->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  return numberOfChar;
}

Expression OppositeNode::shallowReduce(const ReductionContext& reductionContext) {
  return Opposite(this).shallowReduce(reductionContext);
}

/* Simplification */

Expression Opposite::shallowReduce(ReductionContext reductionContext) {
  Expression result = SimplificationHelper::defaultShallowReduce(*this, &reductionContext);
  if (!result.isUninitialized()) {
    return result;
  }
  Expression child = childAtIndex(0);
  result = Multiplication::Builder(Rational::Builder(-1), child);
  replaceWithInPlace(result);
  return result.shallowReduce(reductionContext);
}

}
