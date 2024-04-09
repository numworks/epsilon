#include <poincare/layout.h>
#include <poincare/old/arc_cosine.h>
#include <poincare/old/complex.h>
#include <poincare/old/conjugate.h>
#include <poincare/old/derivative.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/power.h>
#include <poincare/old/rational.h>
#include <poincare/old/serialization_helper.h>
#include <poincare/old/simplification_helper.h>
#include <poincare/old/square_root.h>
#include <poincare/old/subtraction.h>

#include <cmath>

namespace Poincare {

int ArcCosineNode::numberOfChildren() const {
  return ArcCosine::s_functionHelper.numberOfChildren();
}

size_t ArcCosineNode::serialize(char* buffer, size_t bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      ArcCosine::s_functionHelper.aliasesList().mainAlias());
}

OExpression ArcCosineNode::shallowReduce(
    const ReductionContext& reductionContext) {
  ArcCosine e = ArcCosine(this);
  return Trigonometry::ShallowReduceInverseFunction(e, reductionContext);
}

bool ArcCosineNode::derivate(const ReductionContext& reductionContext,
                             Symbol symbol, OExpression symbolValue) {
  return ArcCosine(this).derivate(reductionContext, symbol, symbolValue);
}

OExpression ArcCosineNode::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return ArcCosine(this).unaryFunctionDifferential(reductionContext);
}

template <typename T>
std::complex<T> ArcCosineNode::computeOnComplex(
    const std::complex<T> c, Preferences::ComplexFormat,
    Preferences::AngleUnit angleUnit) {
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
      result.imag(-result.imag());  // other side of the cut
    }
  }
  result =
      ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result, c);
  return Trigonometry::ConvertRadianToAngleUnit(result, angleUnit);
}

bool ArcCosine::derivate(const ReductionContext& reductionContext,
                         Symbol symbol, OExpression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue,
                                    reductionContext);
  return true;
}

OExpression ArcCosine::unaryFunctionDifferential(
    const ReductionContext& reductionContext) {
  return Power::Builder(
      Multiplication::Builder(
          Rational::Builder(-1),
          Trigonometry::UnitConversionFactor(reductionContext.angleUnit(),
                                             Preferences::AngleUnit::Radian),
          SquareRoot::Builder(Subtraction::Builder(
              Rational::Builder(1),
              Power::Builder(childAtIndex(0).clone(), Rational::Builder(2))))),
      Rational::Builder(-1));
}

}  // namespace Poincare
