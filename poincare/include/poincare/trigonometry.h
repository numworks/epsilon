#ifndef POINCARE_TRIGONOMETRY_H
#define POINCARE_TRIGONOMETRY_H

#include <poincare/expression.h>
#include <poincare/evaluation.h>

namespace Poincare {

class Trigonometry final {
public:
  enum class Function {
    Cosine = 0,
    Sine = 1,
  };
  static double PiInAngleUnit(Preferences::AngleUnit angleUnit);
  static float characteristicXRange(const Expression & e, Context * context, Preferences::AngleUnit angleUnit);
  static bool isDirectTrigonometryFunction(const Expression & e);
  static bool isInverseTrigonometryFunction(const Expression & e);
  static bool AreInverseFunctions(const Expression & directFunction, const Expression & inverseFunction);
  static bool ExpressionIsEquivalentToTangent(const Expression & e);
  static Expression shallowReduceDirectFunction(Expression & e, ExpressionNode::ReductionContext reductionContext);
  static Expression shallowReduceInverseFunction(Expression & e,  ExpressionNode::ReductionContext reductionContext);
  static Expression table(const Expression e, ExpressionNode::Type type,  ExpressionNode::ReductionContext reductionContext); // , Function f, bool inverse
  template <typename T> static std::complex<T> ConvertToRadian(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  template <typename T> static std::complex<T> ConvertRadianToAngleUnit(const std::complex<T> c, Preferences::AngleUnit angleUnit);
};

}

#endif
