#include <poincare/kmat.h>
#include <poincare/constant.h>
#include <poincare/serialization_helper.h>
#include <poincare/layout_helper.h>
#include <poincare/expression.h>
#include "parsing/token.h"
#include <poincare/integer.h>
#include <poincare/expression.h>
#include <poincare/rational.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/symbol.h>
#include <utility>

namespace Poincare {

constexpr Expression::FunctionHelper KMat::s_functionHelper;

int KMatNode::numberOfChildren() const { return KMat::s_functionHelper.numberOfChildren(); }

Layout KMatNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(KMat(this), floatDisplayMode, numberOfSignificantDigits, KMat::s_functionHelper.name());
}

int KMatNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, KMat::s_functionHelper.name());
}

Expression KMatNode::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  return KMat(this).shallowReduce(reductionContext);
}

Expression KMat::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);
  if (c0.type() == ExpressionNode::Type::Rational) {
    Rational r0 = static_cast<Rational &>(c0);
    if (!r0.isInteger() || r0.signedIntegerNumerator().isNegative()) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c1.type() == ExpressionNode::Type::Rational) {
    Rational r1 = static_cast<Rational&>(c1);
    if (!r1.isInteger() || r1.signedIntegerNumerator().isNegative()) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c0.type() != ExpressionNode::Type::Rational || c1.type() != ExpressionNode::Type::Rational) {
    return *this;
  }

  Rational r0 = static_cast<Rational&>(c0);
  Rational r1 = static_cast<Rational&>(c1);

  Integer w = r0.signedIntegerNumerator();
  Integer h = r1.signedIntegerNumerator();
  uint32_t size = *Integer::Multiplication(w,h).digits();
  Matrix matrix = Matrix::Builder();
  matrix.addChildAtIndexInPlace(childAtIndex(2).clone(), 0, 0);
  for (uint32_t i = 1; i < size; i++) {
    matrix.addChildAtIndexInPlace(childAtIndex(2).clone(), matrix.numberOfChildren(), matrix.numberOfChildren());
  }
  matrix.setDimensions(*w.digits(), *h.digits());
  replaceWithInPlace(matrix);
  return std::move(matrix);
}

}
