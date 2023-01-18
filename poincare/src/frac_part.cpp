#include <poincare/frac_part.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/rational.h>
#include <cmath>

namespace Poincare {

int FracPartNode::numberOfChildren() const { return FracPart::s_functionHelper.numberOfChildren(); }

Layout FracPartNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(FracPart(this), floatDisplayMode, numberOfSignificantDigits, FracPart::s_functionHelper.aliasesList().mainAlias(), context);
}

int FracPartNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, FracPart::s_functionHelper.aliasesList().mainAlias());
}

Expression FracPartNode::shallowReduce(const ReductionContext& reductionContext) {
  return FracPart(this).shallowReduce(reductionContext);
}

template<typename T>
Complex<T> FracPartNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  if (c.imag() != 0) {
    return Complex<T>::RealUndefined();
  }
  return Complex<T>::Builder(c.real()-std::floor(c.real()));
}


Expression FracPart::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() != ExpressionNode::Type::Rational) {
    return *this;
  }
  Rational r = static_cast<Rational &>(c);
  IntegerDivision div = Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
  assert(!div.remainder.isOverflow());
  Integer rDenominator = r.integerDenominator();
  Expression result = Rational::Builder(div.remainder, rDenominator);
  replaceWithInPlace(result);
  return result;
}

}
