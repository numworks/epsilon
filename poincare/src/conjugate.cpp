#include <poincare/conjugate.h>
#include <poincare/conjugate_layout.h>
#include <poincare/complex_cartesian.h>
#include <poincare/complex_polar.h>
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

ComplexCartesian ConjugateNode::complexCartesian(Context & context, Preferences::AngleUnit angleUnit) const {
  ComplexCartesian childCartesian = childAtIndex(0)->complexCartesian(context, angleUnit);
  if (childCartesian.isUninitialized()) {
    return ComplexCartesian();
  }
  return ComplexCartesian::Builder(
    childCartesian.real(),
    Multiplication(Rational(-1), childCartesian.imag()).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation)
    );
}

ComplexPolar ConjugateNode::complexPolar(Context & context, Preferences::AngleUnit angleUnit) const {
  ComplexPolar childPolar = childAtIndex(0)->complexPolar(context, angleUnit);
  if (childPolar.isUninitialized()) {
    return ComplexPolar();
  }
  return ComplexPolar::Builder(
    childPolar.norm(),
    Multiplication(Rational(-1), childPolar.arg()).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation)
    );
}

Layout ConjugateNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return ConjugateLayout(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int ConjugateNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Conjugate::s_functionHelper.name());
}

Expression ConjugateNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Conjugate(this).shallowReduce(context, angleUnit);
}

template<typename T>
Complex<T> ConjugateNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  return Complex<T>(std::conj(c));
}

Expression Conjugate::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
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
  if (c.type() == ExpressionNode::Type::Rational) {
    replaceWithInPlace(c);
    return c;
  }
  return *this;
}

}
