#include <poincare/prediction_interval.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <assert.h>
#include <cmath>
#include <utility>

namespace Poincare {

constexpr Expression::FunctionHelper PredictionInterval::s_functionHelper;

int PredictionIntervalNode::numberOfChildren() const { return PredictionInterval::s_functionHelper.numberOfChildren(); }

Layout PredictionIntervalNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(PredictionInterval(this), floatDisplayMode, numberOfSignificantDigits, PredictionInterval::s_functionHelper.name());
}

int PredictionIntervalNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, PredictionInterval::s_functionHelper.name());
}


Expression PredictionIntervalNode::shallowReduce(ReductionContext reductionContext) {
  return PredictionInterval(this).shallowReduce(reductionContext);
}

template<typename T>
Evaluation<T> PredictionIntervalNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> pInput = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> nInput = childAtIndex(1)->approximate(T(), context, complexFormat, angleUnit);
  T p = static_cast<Complex<T> &>(pInput).toScalar();
  T n = static_cast<Complex<T> &>(nInput).toScalar();
  if (std::isnan(p) || std::isnan(n) || n != (int)n || n < 0 || p < 0 || p > 1) {
    return Complex<T>::RealUndefined();
  }
  std::complex<T> operands[2];
  operands[0] = std::complex<T>(p - (T)1.96*std::sqrt(p*((T)1.0-p))/std::sqrt(n));
  operands[1] = std::complex<T>(p + (T)1.96*std::sqrt(p*((T)1.0-p))/std::sqrt(n));
  return MatrixComplex<T>::Builder(operands, 1, 2);
}


Expression PredictionInterval::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    e = e.defaultHandleUnitsInChildren();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);
  if (c0.deepIsMatrix(reductionContext.context()) || c1.deepIsMatrix(reductionContext.context())) {
    return replaceWithUndefinedInPlace();
  }
  if (c0.type() == ExpressionNode::Type::Rational) {
    Rational r0 = static_cast<Rational &>(c0);
    if (r0.sign() == ExpressionNode::Sign::Negative || Integer::NaturalOrder(r0.unsignedIntegerNumerator(), r0.integerDenominator()) > 0) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c1.type() == ExpressionNode::Type::Rational) {
    Rational r1 = static_cast<Rational &>(c1);
    if (!r1.isInteger() || r1.sign() == ExpressionNode::Sign::Negative) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c0.type() != ExpressionNode::Type::Rational || c1.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r0 = static_cast<Rational &>(c0);
  Rational r1 = static_cast<Rational &>(c1);
  if (!r1.isInteger() || r1.sign() == ExpressionNode::Sign::Negative || r0.sign() == ExpressionNode::Sign::Negative || Integer::NaturalOrder(r0.unsignedIntegerNumerator(), r0.integerDenominator()) > 0) {
    return replaceWithUndefinedInPlace();
  }
  /* [r0-1.96*sqrt(r0*(1-r0)/r1), r0+1.96*sqrt(r0*(1-r0)/r1)]*/
  // Compute numerator = r0*(1-r0)
  Integer factorNumerator = Integer::Subtraction(r0.integerDenominator(), r0.unsignedIntegerNumerator());
  Integer factorDenominator = r0.integerDenominator();
  Rational numerator = Rational::Multiplication(r0, Rational::Builder(factorNumerator, factorDenominator));
  if (numerator.numeratorOrDenominatorIsInfinity()) {
    return *this;
  }
  // Compute sqr = sqrt(r0*(1-r0)/r1)
  Expression sqr = Power::Builder(Division::Builder(numerator, r1), Rational::Builder(1, 2));
  Expression m = Multiplication::Builder(Rational::Builder(196, 100), sqr);
  Matrix matrix = Matrix::Builder();
  matrix.addChildAtIndexInPlace(Addition::Builder(r0.clone(), Multiplication::Builder(Rational::Builder(-1), m.clone())), 0, 0);
  matrix.addChildAtIndexInPlace(Addition::Builder(r0.clone(), m), 1, 1);
  matrix.setDimensions(1, 2);
  replaceWithInPlace(matrix);
  matrix.deepReduceChildren(reductionContext);
  return std::move(matrix);
}

}
