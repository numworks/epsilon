#include <limits.h>
#include <poincare/layout.h>
#include <poincare/old/integer.h>
#include <poincare/old/matrix.h>
#include <poincare/old/matrix_complex.h>
#include <poincare/old/matrix_identity.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/undefined.h>

#include <cmath>

namespace Poincare {

int MatrixIdentityNode::numberOfChildren() const {
  return MatrixIdentity::s_functionHelper.numberOfChildren();
}

OExpression MatrixIdentityNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return MatrixIdentity(this).shallowReduce(reductionContext);
}

size_t MatrixIdentityNode::serialize(
    char* buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      MatrixIdentity::s_functionHelper.aliasesList().mainAlias());
}

template <typename T>
Evaluation<T> MatrixIdentityNode::templatedApproximate(
    const ApproximationContext& approximationContext) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), approximationContext);
  T r = input.toScalar();  // Undefined if the child is not real
  if (!std::isnan(r) && !std::isinf(r) &&
      r > static_cast<T>(0.0)           // The child is defined and positive
      && std::ceil(r) == std::floor(r)  // The child is an integer
      && r < ((float)INT_MAX))          // The child is not too big
  {
    return MatrixComplex<T>::CreateIdentity((int)r);
  }
  return Complex<T>::Undefined();
}

OExpression MatrixIdentity::shallowReduce(ReductionContext reductionContext) {
  {
    OExpression e = SimplificationHelper::defaultShallowReduce(
        *this, &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  OExpression c = childAtIndex(0);
  if (c.otype() != ExpressionNode::Type::Rational ||
      !static_cast<Rational&>(c).isInteger()) {
    return *this;
  }
  Integer dimension = static_cast<Rational&>(c).signedIntegerNumerator();
  if (dimension.isNegative() || dimension.isZero()) {
    return replaceWithUndefinedInPlace();
  }
  if (Integer::NaturalOrder(dimension, Integer(15)) >
      0) {  // For now, after 15 it would fill the pool
    return *this;
  }
  int dim = dimension.extractedInt();
  assert(dim != 0);
  OExpression result = OMatrix::CreateIdentity(dim);
  replaceWithInPlace(result);
  return result;
}

}  // namespace Poincare
