#include <poincare/conjugate.h>
#include <poincare/conjugate_layout.h>
#include <poincare/complex_cartesian.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <assert.h>
#include <cmath>
#include <utility>

namespace Poincare {

int ConjugateNode::numberOfChildren() const { return Conjugate::s_functionHelper.numberOfChildren(); }

Layout ConjugateNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return ConjugateLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits, context));
}

int ConjugateNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Conjugate::s_functionHelper.aliasesList().mainAlias());
}

Expression ConjugateNode::shallowReduce(const ReductionContext& reductionContext) {
  return Conjugate(this).shallowReduce(reductionContext);
}

template<typename T>
Complex<T> ConjugateNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  return Complex<T>::Builder(std::conj(c));
}

Expression Conjugate::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
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
  if (c.isReal(reductionContext.context(), reductionContext.shouldCheckMatrices())) {
    replaceWithInPlace(c);
    return c;
  }
  if (c.type() == ExpressionNode::Type::ComplexCartesian) {
    ComplexCartesian complexChild = static_cast<ComplexCartesian &>(c);
    Multiplication m = Multiplication::Builder(Rational::Builder(-1), complexChild.imag());
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


}
