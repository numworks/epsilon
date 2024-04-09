#include <assert.h>
#include <float.h>
#include <ion.h>
#include <poincare/layout.h>
#include <poincare/old/ceiling.h>
#include <poincare/old/constant.h>
#include <poincare/old/float.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/symbol.h>

#include <cmath>

namespace Poincare {

int CeilingNode::numberOfChildren() const {
  return Ceiling::s_functionHelper.numberOfChildren();
}

size_t CeilingNode::serialize(char* buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Ceiling::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
std::complex<T> CeilingNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0) {
    return complexRealNAN<T>();
  }
  /* Assume low deviation from natural numbers are errors */
  T delta = std::fabs((std::round(c.real()) - c.real()) / c.real());
  if (delta <= Float<T>::Epsilon()) {
    return std::round(c.real());
  }
  return std::ceil(c.real());
}

OExpression CeilingNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Ceiling(this).shallowReduce(reductionContext);
}

OExpression Ceiling::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::ExtractUnitsOfFirstChild,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }

  OExpression c = childAtIndex(0);
  if (c.otype() == ExpressionNode::Type::Rational) {
    Rational r = c.convert<Rational>();
    IntegerDivision div =
        Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
    assert(!div.remainder.isOverflow());
    if (div.remainder.isZero()) {
      OExpression result = Rational::Builder(div.quotient);
      replaceWithInPlace(result);
      return result;
    }
    Integer result = Integer::Addition(div.quotient, Integer(1));
    if (result.isOverflow()) {
      return *this;
    }
    OExpression rationalResult = Rational::Builder(result);
    replaceWithInPlace(rationalResult);
    return rationalResult;
  }
  return shallowReduceUsingApproximation(reductionContext);
}

}  // namespace Poincare
