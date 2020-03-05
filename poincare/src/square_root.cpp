#include <poincare/square_root.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/nth_root_layout.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/sign_function.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>

#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Poincare {

constexpr Expression::FunctionHelper SquareRoot::s_functionHelper;

int SquareRootNode::numberOfChildren() const { return SquareRoot::s_functionHelper.numberOfChildren(); }

Layout SquareRootNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return NthRootLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int SquareRootNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, SquareRoot::s_functionHelper.name());
}

template<typename T>
Complex<T> SquareRootNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::sqrt(c);
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result, std::complex<T>(std::log(std::abs(c)), std::arg(c))));
}

Expression SquareRootNode::shallowReduce(ReductionContext reductionContext) {
  return SquareRoot(this).shallowReduce(reductionContext);
}

Expression SquareRoot::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.deepIsMatrix(reductionContext.context())) {
    return replaceWithUndefinedInPlace();
  }
  Power p = Power::Builder(c, Rational::Builder(1, 2));
  replaceWithInPlace(p);
  return p.shallowReduce(reductionContext);
}

}
