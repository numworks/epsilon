#include <poincare/division_quotient.h>
#include <poincare/approximation_helper.h>
#include <poincare/infinity.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>
#include <cmath>

namespace Poincare {

int DivisionQuotientNode::numberOfChildren() const { return DivisionQuotient::s_functionHelper.numberOfChildren(); }

ExpressionNode::Sign DivisionQuotientNode::sign(Context * context) const {
  ExpressionNode::Sign numeratorSign = childAtIndex(0)->sign(context);
  ExpressionNode::Sign denominatorSign = childAtIndex(1)->sign(context);
  if (numeratorSign == ExpressionNode::Sign::Unknown || denominatorSign == ExpressionNode::Sign::Unknown) {
    return ExpressionNode::Sign::Unknown;
  }
  return numeratorSign == denominatorSign ? ExpressionNode::Sign::Positive : ExpressionNode::Sign::Negative;
}

Expression DivisionQuotientNode::shallowReduce(const ReductionContext& reductionContext) {
  return DivisionQuotient(this).shallowReduce(reductionContext);
}

Layout DivisionQuotientNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(DivisionQuotient(this), floatDisplayMode, numberOfSignificantDigits, DivisionQuotient::s_functionHelper.name());
}
int DivisionQuotientNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, DivisionQuotient::s_functionHelper.name());
}

template<typename T>
Evaluation<T> DivisionQuotientNode::templatedApproximate(const ApproximationContext& approximationContext) const {
  return ApproximationHelper::Map<T>(
      this,
      approximationContext,
      [] (const std::complex<T> * c, int numberOfComplexes, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, void * ctx) {
        assert(numberOfComplexes == 2);
        T f1 = ComplexNode<T>::ToScalar(c[0]);
        T f2 = ComplexNode<T>::ToScalar(c[1]);
        if (std::isnan(f1) || std::isnan(f2) || f1 != (int)f1 || f2 != (int)f2) {
          return Complex<T>::RealUndefined();
        }
        return Complex<T>::Builder(DivisionQuotient::TemplatedQuotient(f1, f2));
      });
  }

Expression DivisionQuotient::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::undefinedOnMatrix(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
    e = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  Expression c1 = childAtIndex(1);
  if (c0.type() == ExpressionNode::Type::Rational) {
    Rational r0 = static_cast<Rational &>(c0);
    if (!r0.isInteger()) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c1.type() == ExpressionNode::Type::Rational) {
    Rational r1 = static_cast<Rational &>(c1);
    if (!r1.isInteger()) {
      return replaceWithUndefinedInPlace();
    }
  }
  if (c0.type() != ExpressionNode::Type::Rational || c1.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r0 = static_cast<Rational &>(c0);
  Rational r1 = static_cast<Rational &>(c1);

  Integer a = r0.signedIntegerNumerator();
  Integer b = r1.signedIntegerNumerator();
  Expression result = Reduce(a, b);
  replaceWithInPlace(result);
  return result;
}

Expression DivisionQuotient::Reduce(const Integer & a, const Integer & b) {
  if (b.isZero()) {
    return Infinity::Builder(a.isNegative());
  }
  Integer result = Integer::Division(a, b).quotient;
  assert(!result.isOverflow());
  return Rational::Builder(result);
}

}
