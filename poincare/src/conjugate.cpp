#include <poincare/conjugate.h>
#include <poincare/conjugate_layout.h>
#include <poincare/complex_cartesian.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/opposite.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Conjugate::s_functionHelper;

int ConjugateNode::numberOfChildren() const { return Conjugate::s_functionHelper.numberOfChildren(); }

Layout ConjugateNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return ConjugateLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int ConjugateNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Conjugate::s_functionHelper.name());
}

Expression ConjugateNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Conjugate(this).shallowReduce(context, complexFormat, angleUnit, target);
}

template<typename T>
Complex<T> ConjugateNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  return Complex<T>::Builder(std::conj(c));
}

Expression Conjugate::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  if (c.isReal(context)) {
    replaceWithInPlace(c);
    return c;
  }
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian &>(c);
    Multiplication m = Multiplication::Builder(Rational::Builder(-1), complexChild.imag());
    complexChild.replaceChildAtIndexInPlace(1, m);
    m.shallowReduce(context, complexFormat, angleUnit, target);
    replaceWithInPlace(complexChild);
    return complexChild;
  }
  if (c.type() == ExpressionNode::Type::Rational) {
    replaceWithInPlace(c);
    return c;
  }
  return *this;
}


}
