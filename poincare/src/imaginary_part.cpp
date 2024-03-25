#include <poincare/complex_cartesian.h>
#include <poincare/imaginary_part.h>
#include <poincare/layout.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

#include <cmath>

namespace Poincare {

int ImaginaryPartNode::numberOfChildren() const {
  return ImaginaryPart::s_functionHelper.numberOfChildren();
}

size_t ImaginaryPartNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ImaginaryPart::s_functionHelper.aliasesList().mainAlias());
}

OExpression ImaginaryPartNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return ImaginaryPart(this).shallowReduce(reductionContext);
}

OExpression ImaginaryPart::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits,
        SimplificationHelper::MatrixReduction::UndefinedOnMatrix,
        SimplificationHelper::ListReduction::DistributeOverLists);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c = childAtIndex(0);
  if (c.isReal(reductionContext.context(),
               reductionContext.shouldCheckMatrices())) {
    OExpression result = Rational::Builder(0);
    replaceWithInPlace(result);
    return result;
  }
  if (c.otype() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian&>(c);
    OExpression i = complexChild.imag();
    replaceWithInPlace(i);
    return i.shallowReduce(reductionContext);
  }
  return *this;
}

}  // namespace Poincare
