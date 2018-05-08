#ifndef POINCARE_TRIGONOMETRY_H
#define POINCARE_TRIGONOMETRY_H

#include <poincare/expression.h>
#include <poincare/evaluation.h>

namespace Poincare {

class Trigonometry  {
public:
  enum class Function {
    Cosine = 0,
    Sine = 1,
  };
  static float characteristicXRange(const Expression * e, Context & context, Expression::AngleUnit angleUnit);
  static Expression * shallowReduceDirectFunction(Expression * e, Context& context, Expression::AngleUnit angleUnit);
  static Expression * shallowReduceInverseFunction(Expression * e, Context& context, Expression::AngleUnit angleUnit);
  static bool ExpressionIsEquivalentToTangent(const Expression * e);
  constexpr static int k_numberOfEntries = 37;
  static Expression * table(const Expression * e, Expression::Type type, Context & context, Expression::AngleUnit angleUnit); // , Function f, bool inverse
  template <typename T> static std::complex<T> ConvertToRadian(const std::complex<T> c, Expression::AngleUnit angleUnit);
  template <typename T> static std::complex<T> ConvertRadianToAngleUnit(const std::complex<T> c, Expression::AngleUnit angleUnit);
  template <typename T> static std::complex<T> RoundToMeaningfulDigits(const std::complex<T> c);
private:
  template <typename T> static T RoundToMeaningfulDigits(T f);
};

}

#endif
