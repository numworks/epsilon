#ifndef POINCARE_TRIGONOMETRY_H
#define POINCARE_TRIGONOMETRY_H

#include <complex.h>
#include <poincare/expression.h>
#include <poincare/preferences.h>

namespace Poincare {

class Trigonometry final {
 public:
  static double PiInAngleUnit(Preferences::AngleUnit angleUnit);
  static double ConvertAngleToRadian(double angle,
                                     Preferences::AngleUnit angleUnit);

  template <typename T>
  static std::complex<T> ConvertToRadian(const std::complex<T> c,
                                         Preferences::AngleUnit angleUnit);
  template <typename T>
  static std::complex<T> ConvertRadianToAngleUnit(
      const std::complex<T> c, Preferences::AngleUnit angleUnit);
  static UserExpression Period(Preferences::AngleUnit angleUnit);

  /* Turn cos(4) into cos(4rad) if the angle unit is rad and cos(π) into
   * cos(π°) if the angle unit is deg, to notify the user of the current
   * angle unit she is using if she's forgetting to switch the angle unit */
  static void DeepAddAngleUnitToAmbiguousDirectFunctions(
      UserExpression& e, Preferences::AngleUnit angleUnit);
};

}  // namespace Poincare

#endif
