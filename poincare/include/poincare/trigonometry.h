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
  static float characteristicXRange(const Expression & e, Context & context, Preferences::AngleUnit angleUnit);
  static Expression shallowReduceDirectFunction(Expression & e, Context& context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  static Expression shallowReduceInverseFunction(Expression & e, Context& context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target);
  static bool ExpressionIsEquivalentToTangent(const Expression & e);
  constexpr static int k_numberOfEntries = 37;
  static Expression table(const Expression e, ExpressionNode::Type type, Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target); // , Function f, bool inverse
  template <typename T> static std::complex<T> ConvertToRadian(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  template <typename T> static std::complex<T> ConvertRadianToAngleUnit(const std::complex<T> c, Preferences::AngleUnit angleUnit);
  template <typename T> static std::complex<T> RoundToMeaningfulDigits(const std::complex<T> result, const std::complex<T> input);
private:
  template <typename T> static T RoundToMeaningfulDigits(T result, T input);
};

}

#endif
