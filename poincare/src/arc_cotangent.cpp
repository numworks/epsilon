#include <poincare/arc_tangent.h>
#include <poincare/addition.h>
#include <poincare/complex.h>
#include <poincare/derivative.h>
#include <poincare/layout_helper.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/trigonometry.h>
#include <cmath>

namespace Poincare {

int ArcCotangentNode::numberOfChildren() const { return ArcCotangent::s_functionHelper.numberOfChildren(); }

template<typename T>
Complex<T> ArcCotangentNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  if (c == static_cast<T>(0.0)) {
    return Complex<T>::Builder(Trigonometry::ConvertRadianToAngleUnit(std::complex<T>(M_PI_2), angleUnit));
  }
  return ArcTangentNode::computeOnComplex<T>(std::complex<T>(1) / c, complexFormat, angleUnit);
}

Layout ArcCotangentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  return LayoutHelper::Prefix(ArcCotangent(this), floatDisplayMode, numberOfSignificantDigits, ArcCotangent::s_functionHelper.aliasesList().mainAlias(), context);
}

int ArcCotangentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ArcCotangent::s_functionHelper.aliasesList().mainAlias());
}

Expression ArcCotangentNode::shallowReduce(const ReductionContext& reductionContext) {
  return ArcCotangent(this).shallowReduce(reductionContext);
}

bool ArcCotangentNode::derivate(const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return ArcCotangent(this).derivate(reductionContext, symbol, symbolValue);
}

Expression ArcCotangentNode::unaryFunctionDifferential(const ReductionContext& reductionContext) {
  return ArcCotangent(this).unaryFunctionDifferential(reductionContext);
}

Expression ArcCotangent::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(
        *this,
        &reductionContext,
        SimplificationHelper::BooleanReduction::UndefinedOnBooleans,
        SimplificationHelper::UnitReduction::BanUnits
    );
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return Trigonometry::shallowReduceInverseAdvancedFunction(*this, reductionContext);
}

bool ArcCotangent::derivate(const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  Derivative::DerivateUnaryFunction(*this, symbol, symbolValue, reductionContext);
  return true;
}

Expression ArcCotangent::unaryFunctionDifferential(const ExpressionNode::ReductionContext& reductionContext) {
  return Multiplication::Builder(
    Power::Builder(
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
    ),
    Rational::Builder(-1)
  );
}

}