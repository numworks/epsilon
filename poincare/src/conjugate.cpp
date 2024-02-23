#include <assert.h>
#include <poincare/complex_cartesian.h>
#include <poincare/conjugate.h>
#include <poincare/conjugate_layout.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>

#include <cmath>
#include <utility>

namespace Poincare {

int ConjugateNode::numberOfChildren() const {
  return Conjugate::s_functionHelper.numberOfChildren();
}

size_t ConjugateNode::serialize(char* buffer, size_t bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      Conjugate::s_functionHelper.aliasesList().mainAlias());
}

OExpression ConjugateNode::shallowReduce(
    const ReductionContext& reductionContext) {
  return Conjugate(this).shallowReduce(reductionContext);
}

template <typename T>
std::complex<T> ConjugateNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  return std::conj(c);
}

OExpression Conjugate::shallowReduce(ReductionContext reductionContext) {
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
    replaceWithInPlace(c);
    return c;
  }
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian&>(c);
    Multiplication m =
        Multiplication::Builder(Rational::Builder(-1), complexChild.imag());
    complexChild.replaceChildAtIndexInPlace(1, m);
    m.shallowReduce(reductionContext);
    replaceWithInPlace(complexChild);
    return std::move(complexChild);
  }
  if (c.type() == ExpressionNode::Type::Rational) {
    replaceWithInPlace(c);
    return c;
  }
  return *this;
}

}  // namespace Poincare
