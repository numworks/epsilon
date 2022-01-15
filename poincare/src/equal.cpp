#include <poincare/equal.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/code_point_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/horizontal_layout.h>
#include <ion.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
}
#include <utility>

namespace Poincare {

Expression EqualNode::shallowReduce(ReductionContext reductionContext) {
  return Equal(this).shallowReduce(reductionContext);
}

Layout EqualNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  HorizontalLayout result = HorizontalLayout::Builder();
  result.addOrMergeChildAtIndex(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), 0, false);
  result.addChildAtIndex(CodePointLayout::Builder('='), result.numberOfChildren(), result.numberOfChildren(), nullptr);
  result.addOrMergeChildAtIndex(childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), result.numberOfChildren(), false);
  return std::move(result);
}

int EqualNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "=");
}

template<typename T>
Evaluation<T> EqualNode::templatedApproximate(ApproximationContext approximationContext) const {
  return Complex<T>::Undefined();
}


Expression Equal::standardEquation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, ExpressionNode::ReductionTarget reductionTarget) const {
  Expression sub = Subtraction::Builder(childAtIndex(0).clone(), childAtIndex(1).clone());
  return sub.reduce(ExpressionNode::ReductionContext(context, complexFormat, angleUnit, unitFormat, reductionTarget));
}

Expression Equal::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  
  Expression e = Equal::Builder(Subtraction::Builder(childAtIndex(0).clone(), childAtIndex(1).clone()).shallowReduce(reductionContext), Rational::Builder(0));
  
  Expression leftSide = e.childAtIndex(0);
  if (leftSide.isUndefined()) {
    return leftSide; // <=> undefined
  }
  if (leftSide.type() == ExpressionNode::Type::Multiplication) { // Simplify multiplication
    Multiplication m = static_cast<Multiplication&>(leftSide);
    int i = 0;
    while (i < m.numberOfChildren()) {
      if (m.childAtIndex(i).nullStatus(reductionContext.context()) == ExpressionNode::NullStatus::NonNull) {
        m.removeChildAtIndexInPlace(i);
      }
      else {
        i++;
      }
    }

    // Replace if 0 child
    if (m.numberOfChildren() == 0) {
      e.replaceChildAtIndexInPlace(0, Rational::Builder(0));
    } else {
      // Squash if one child 
      Expression result = m.squashUnaryHierarchyInPlace();
      if (result != *this) {
        e.replaceChildAtIndexInPlace(0, result);
      }
    }
  }
  
  if (leftSide.isIdenticalTo(e.childAtIndex(1))) {
    Expression result = Rational::Builder(1);
    replaceWithInPlace(result);
    return result;
  }

  return e;
}

}
