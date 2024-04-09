#include <poincare/layout.h>
#include <poincare/old/arc_sine.h>
#include <poincare/old/complex.h>
#include <poincare/old/conjugate.h>
#include <poincare/old/derivative.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/square_root.h>
#include <poincare/old/subtraction.h>

#include <cmath>

namespace Poincare {

int ArcSineNode::numberOfChildren() const {
  return ArcSine::s_functionHelper.numberOfChildren();
}

size_t ArcSineNode::serialize(char* buffer, size_t bufferSize,
                              Preferences::PrintFloatMode floatDisplayMode,
                              int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ArcSine::s_functionHelper.aliasesList().mainAlias());
}

OExpression ArcSineNode::shallowReduce(
    const ReductionContext& reductionContext) {
  ArcSine e = ArcSine(this);
  return Trigonometry::ShallowReduceInverseFunction(e, reductionContext);
}

bool ArcSineNode::derivate(const ReductionContext& reductionContext,
                           Symbol symbol, OExpression symbolValue) {
  return ArcSine(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression ArcSineNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return ArcSine(this).unaryFunctionDifferential(reductionContext);
}

template <typename T>
std::complex<T> ArcSineNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
  std::complex<T> result;
  if (c.imag() == 0 && std::fabs(c.real()) <= static_cast<T>(1.0)) {
    /* asin: [-1;1] -> R
     * In these cases we rather use std::asin(double) because asin on complexes
     * is not as precise as asin on double in std library. For instance,
     * - asin(complex<double>(0.03,0.0) = complex(0.0300045,1.11022e-16)
     * - asin(0.03) = 0.0300045 */
    result = std::asin(c.real());
  } else {
    result = std::asin(c);
    /* asin has a branch cut on ]-inf, -1[U]1, +inf[: it is then multivalued on
     * this cut. We followed the convention chosen by the lib c++ of llvm on
     * ]-inf+0i, -1+0i[ (warning: asin takes the other side of the cut values on
     * ]-inf-0i, -1-0i[) and choose the values on ]1+0i, +inf+0i[ to comply with
     * asin(-x) = -asin(x) and tan(asin(x)) = x/sqrt(1-x^2). */
    if (c.imag() == 0 && c.real() > 1) {
      result.imag(-result.imag());  // other side of the cut
    }
  }
  result =
      ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result, c);
  return Trigonometry::ConvertRadianToAngleUnit(result, angleUnit);
}

bool ArcSine::derivate(const ReductionContext& reductionContext, Symbol symbol,
                       OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression ArcSine::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Power::Builder(
      Multiplication::Builder(
          Trigonometry::UnitConversionFactor(reductionContext.angleUnit(),
                                             Preferences::AngleUnit::Radian),
          SquareRoot::Builder(Subtraction::Builder(
              Rational::Builder(1),
              Power::Builder(childAtIndex(0).clone(), Rational::Builder(2))))),
      Rational::Builder(-1));
}

}  // namespace Poincare
