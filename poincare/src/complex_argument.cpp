#include <poincare/complex_argument.h>
#include <poincare/complex_cartesian.h>
#include <poincare/constant.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/undefined.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

int ComplexArgumentNode::numberOfChildren() const {
  return ComplexArgument::s_functionHelper.numberOfChildren();
}

OLayout ComplexArgumentNode::createLayout(
    Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits,
    Context* context) const {
  return LayoutHelper::Prefix(
      ComplexArgument(this), floatDisplayMode, numberOfSignificantDigits,
      ComplexArgument::s_functionHelper.aliasesList().mainAlias(), context);
}

size_t ComplexArgumentNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ComplexArgument::s_functionHelper.aliasesList().mainAlias());
}

Expression ComplexArgumentNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ComplexArgument(this).shallowReduce(reductionContext);
}

template <typename T>
std::complex<T> ComplexArgumentNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  return std::arg(c);
}

Expression ComplexArgument::shallowReduce(ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian&>(c);
    Expression childArg = complexChild.argument(reductionContext);
    replaceWithInPlace(childArg);
    return childArg.shallowReduce(reductionContext);
  }
  Context* context = reductionContext.context();
  Expression res;
  if (c.isNull(context) == TrinaryBoolean::True) {
    res = Undefined::Builder();
  } else if (c.isPositive(context) == TrinaryBoolean::True) {
    res = Rational::Builder(0);
  } else if (c.isPositive(context) == TrinaryBoolean::False) {
    res = Constant::PiBuilder();
  } else {
    ApproximationContext approximationContext(reductionContext, true);
    double approximation = c.approximateToScalar<double>(approximationContext);
    if (approximation < 0.0) {
      res = Constant::PiBuilder();
    } else if (approximation > 0.0) {
      res = Rational::Builder(0);
    } else if (approximation == 0.0) {
      res = Undefined::Builder();
    }  // else, approximation is NaN
  }
  if (res.isUninitialized()) {
    return *this;
  } else {
    replaceWithInPlace(res);
    return res;
  }
}

}  // namespace Poincare
