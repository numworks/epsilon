#include <poincare/arc_cosine.h>
#include <poincare/complex.h>
#include <poincare/conjugate.h>
#include <poincare/derivative.h>
#include <poincare/layout_helper.h>
#include <poincare/multiplication.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/square_root.h>
#include <poincare/subtraction.h>
#include <cmath>

namespace Poincare {

int ArcCosineNode::numberOfChildren() const { return ArcCosine::s_functionHelper.numberOfChildren(); }

Layout ArcCosineNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ArcCosine(this), floatDisplayMode, numberOfSignificantDigits, ArcCosine::s_functionHelper.name());
}

int ArcCosineNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ArcCosine::s_functionHelper.name());
}

Expression ArcCosineNode::shallowReduce(const ReductionContext& reductionContext) {
  return ArcCosine(this).shallowReduce(reductionContext);
}

bool ArcCosineNode::derivate(const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return ArcCosine(this).derivate(reductionContext, symbol, symbolValue);
}

Expression ArcCosineNode::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return ArcCosine(this).unaryFunctionDifferential(reductionContext);
}

template<typename T>
Complex<T> ArcCosineNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> result;
  if (c.imag() == 0 && std::fabs(c.real()) <= static_cast<T>(1.0)) {
    /* acos: [-1;1] -> R
     * In these cases we rather use std::acos(double) because acos on complexes
     * is not as precise as pow on double in std library. For instance,
     * - acos(complex<double>(0.03,0.0) = complex(1.54079,-1.11022e-16)
     * - acos(0.03) = 1.54079 */
    result = std::acos(c.real());
  } else {
    result = std::acos(c);
    /* acos has a branch cut on ]-inf, -1[U]1, +inf[: it is then multivalued on
     * this cut. We followed the convention chosen by the lib c++ of llvm on
     * ]-inf+0i, -1+0i[ (warning: acos takes the other side of the cut values on
     * ]-inf-0i, -1-0i[) and choose the values on ]1+0i, +inf+0i[ to comply with
     * acos(-x) = π - acos(x) and tan(acos(x)) = sqrt(1-x^2)/x. */
    if (c.imag() == 0 && c.real() > 1) {
      result.imag(-result.imag()); // other side of the cut
    }
  }
  result = ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result, c);
  return Complex<T>::Builder(Trigonometry::ConvertRadianToAngleUnit(result, angleUnit));
}


Expression ArcCosine::shallowReduce(const ExpressionNode::ReductionContext& reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return Trigonometry::shallowReduceInverseFunction(*this, reductionContext);
}

bool ArcCosine::derivate(const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue, reductionContext);
  return true;
}

Expression ArcCosine::unaryFunctionDifferential(const ExpressionNode::ReductionContext& reductionContext) {
  return Power::Builder(
      Multiplication::Builder(
        Rational::Builder(-1),
        Trigonometry::UnitConversionFactor(
          reductionContext.angleUnit(),
          Preferences::AngleUnit::Radian
          ),
        SquareRoot::Builder(
          Subtraction::Builder(
            Rational::Builder(1),
            Power::Builder(
              childAtIndex(0).clone(),
              Rational::Builder(2)
              )
            )
          )
        ),
      Rational::Builder(-1)
      );
}

}
