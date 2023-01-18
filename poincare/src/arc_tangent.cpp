#include <poincare/arc_tangent.h>
#include <poincare/addition.h>
#include <poincare/complex.h>
#include <poincare/derivative.h>
#include <poincare/layout_helper.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/square_root.h>
#include <cmath>

namespace Poincare {

int ArcTangentNode::numberOfChildren() const { return ArcTangent::s_functionHelper.numberOfChildren(); }

Layout ArcTangentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(ArcTangent(this), floatDisplayMode, numberOfSignificantDigits, ArcTangent::s_functionHelper.aliasesList().mainAlias(), context);
}

int ArcTangentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ArcTangent::s_functionHelper.aliasesList().mainAlias());
}

template<typename T>
Complex<T> ArcTangentNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> result;
  if (c.imag() == 0 && std::fabs(c.real()) <= static_cast<T>(1.0)) {
    /* atan: R -> R
     * In these cases we rather use std::atan(double) because atan on complexes
     * is not as precise as atan on double in std library. For instance,
     * - atan(complex<double>(0.01,0.0) = complex(9.9996666866652E-3,5.5511151231258E-17)
     * - atan(0.03) = 9.9996666866652E-3 */
    result = std::atan(c.real());
  } else {
    result = std::atan(c);
    /* atan has a branch cut on ]-inf*i, -i[U]i, +inf*i[: it is then multivalued
     * on this cut. We followed the convention chosen by the lib c++ of llvm on
     * ]-i+0, -i*inf+0[ (warning: atan takes the other side of the cut values on
     * ]-i+0, -i*inf+0[) and choose the values on ]-inf*i, -i[ to comply with
     * atan(-x) = -atan(x) and sin(atan(x)) = x/sqrt(1+x^2). */
    if (c.real() == 0 && c.imag() < -1) {
      result.real(-result.real()); // other side of the cut
    }
  }
  result = ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result, c);
  return Complex<T>::Builder(Trigonometry::ConvertRadianToAngleUnit(result, angleUnit));
}

Expression ArcTangentNode::shallowReduce(const ReductionContext& reductionContext) {
  ArcTangent e = ArcTangent(this);
  return Trigonometry::shallowReduceInverseFunction(e, reductionContext);
}

bool ArcTangentNode::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return ArcTangent(this).derivate(reductionContext, symbol, symbolValue);
}

Expression ArcTangentNode::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return ArcTangent(this).unaryFunctionDifferential(reductionContext);
}

bool ArcTangent::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue, reductionContext);
  return true;
}

Expression ArcTangent::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return Power::Builder(
      Multiplication::Builder(
        Trigonometry::UnitConversionFactor(
          reductionContext.angleUnit(),
          Preferences::AngleUnit::Radian
          ),
        Addition::Builder(
          Rational::Builder(1),
          Power::Builder(
            childAtIndex(0).clone(),
            Rational::Builder(2)
            )
          )
        ),
      Rational::Builder(-1)
      );
}

}
