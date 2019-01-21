#include <poincare/sign_function.h>
#include <poincare/complex_cartesian.h>
#include <poincare/rational.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <ion.h>
#include <assert.h>
#include <math.h>

namespace Poincare {

constexpr Expression::FunctionHelper SignFunction::s_functionHelper;

int SignFunctionNode::numberOfChildren() const { return SignFunction::s_functionHelper.numberOfChildren(); }

ExpressionNode::Sign SignFunctionNode::sign(Context * context) const {
  return childAtIndex(0)->sign(context);
}

Expression SignFunctionNode::setSign(Sign s, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  SignFunction sign(this);
  Rational r(s == ExpressionNode::Sign::Positive ? 1 : -1);
  sign.replaceWithInPlace(r);
  return r;
}

Layout SignFunctionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(SignFunction(this), floatDisplayMode, numberOfSignificantDigits, SignFunction::s_functionHelper.name());
}

int SignFunctionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, SignFunction::s_functionHelper.name());
}

Expression SignFunctionNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return SignFunction(this).shallowReduce(context, complexFormat, angleUnit, target);
}

template<typename T>
Complex<T> SignFunctionNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0 || std::isnan(c.real())) {
    return Complex<T>::Undefined();
  }
  if (c.real() == 0) {
    return Complex<T>(0.0);
  }
  if (c.real() < 0) {
    return Complex<T>(-1.0);
  }
  return Complex<T>(1.0);
}

Expression SignFunction::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(*this, context, angleUnit);
  }
#endif
  Rational resultSign(1);
  Expression child = childAtIndex(0);
  ExpressionNode::Sign s = child.sign(&context);
  if (s == ExpressionNode::Sign::Negative) {
    resultSign = Rational(-1);
  } else {
    Evaluation<float> childApproximated = child.node()->approximate(1.0f, context, complexFormat, angleUnit);
    assert(childApproximated.type() == EvaluationNode<float>::Type::Complex);
    Complex<float> c = static_cast<Complex<float>&>(childApproximated);
    if (std::isnan(c.imag()) || std::isnan(c.real()) || c.imag() != 0) {
      // c's approximation has no sign (c is complex or NAN)
      if (target == ExpressionNode::ReductionTarget::User && s == ExpressionNode::Sign::Positive) {
        // For the user, we want sign(abs(x)) = 1
      } else {
        // sign(-x) = -sign(x)
        Expression oppChild = child.makePositiveAnyNegativeNumeralFactor(context, complexFormat, angleUnit, target);
        if (oppChild.isUninitialized()) {
          return *this;
        }
        Expression sign = *this;
        Multiplication m(Rational(-1));
        replaceWithInPlace(m);
        m.addChildAtIndexInPlace(sign, 1, 1); // sign does not need to be shallowReduced because -x = NAN --> x = NAN
        return m; // m does not need to be shallowReduced, -1*sign cannot be reduced
      }
    } else if (c.real() < 0) {
      resultSign = Rational(-1);
    } else if (c.real() == 0) {
      resultSign = Rational(0);
    }
  }
  replaceWithInPlace(resultSign);
  return resultSign;
}

}
