#include <poincare/tangent.h>
#include <poincare/cosine.h>
#include <poincare/derivative.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>

#include <poincare/sine.h>
#include <poincare/trigonometry.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Tangent::s_functionHelper;

int TangentNode::numberOfChildren() const { return Tangent::s_functionHelper.numberOfChildren(); }

Layout TangentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Tangent(this), floatDisplayMode, numberOfSignificantDigits, Tangent::s_functionHelper.name());
}

int TangentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Tangent::s_functionHelper.name());
}

template<typename T>
Complex<T> TangentNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> angleInput = Trigonometry::ConvertToRadian(c, angleUnit);
  std::complex<T> res = std::tan(angleInput);
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(res, angleInput));
}

Expression TangentNode::shallowReduce(ReductionContext reductionContext) {
  return Tangent(this).shallowReduce(reductionContext);
}

bool TangentNode::derivate(ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  return Tangent(this).derivate(reductionContext, symbol, symbolValue);
}

Expression TangentNode::unaryFunctionDifferential() {
  return Tangent(this).unaryFunctionDifferential();
}

Expression Tangent::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }

  Expression newExpression = Trigonometry::shallowReduceDirectFunction(*this, reductionContext);
  if (newExpression.type() == ExpressionNode::Type::Tangent) {
    Sine s = Sine::Builder(newExpression.childAtIndex(0).clone());
    Cosine c = Cosine::Builder(newExpression.childAtIndex(0));
    Division d = Division::Builder(s, c);
    s.shallowReduce(reductionContext);
    c.shallowReduce(reductionContext);
    newExpression.replaceWithInPlace(d);
    return d.shallowReduce(reductionContext);
  }
  return newExpression;
}

bool Tangent::derivate(ExpressionNode::ReductionContext reductionContext, Expression symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue);
  return true;
}

Expression Tangent::unaryFunctionDifferential() {
  return Power::Builder(Cosine::Builder(childAtIndex(0).clone()), Rational::Builder(-2));
}

}
