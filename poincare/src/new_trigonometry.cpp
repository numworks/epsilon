#include <poincare/new_trigonometry.h>
#include <poincare/src/expression/angle.h>

namespace Poincare {

double NewTrigonometry::PiInAngleUnit(Preferences::AngleUnit angleUnit) {
  switch (angleUnit) {
    case Preferences::AngleUnit::Radian:
      return M_PI;
    case Preferences::AngleUnit::Degree:
      return 180.0;
    default:
      assert(angleUnit == Preferences::AngleUnit::Gradian);
      return 200.0;
  }
}

double NewTrigonometry::ConvertAngleToRadian(double angle,
                                             Preferences::AngleUnit angleUnit) {
  return angleUnit != Preferences::AngleUnit::Radian
             ? angle * M_PI / PiInAngleUnit(angleUnit)
             : angle;
}

template <typename T>
std::complex<T> NewTrigonometry::ConvertToRadian(
    const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (angleUnit != Preferences::AngleUnit::Radian) {
    return c * std::complex<T>((T)M_PI / (T)PiInAngleUnit(angleUnit));
  }
  return c;
}

template <typename T>
std::complex<T> NewTrigonometry::ConvertRadianToAngleUnit(
    const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (angleUnit != Preferences::AngleUnit::Radian) {
    return c * std::complex<T>((T)PiInAngleUnit(angleUnit) / (T)M_PI);
  }
  return c;
}

UserExpression NewTrigonometry::Period(Preferences::AngleUnit angleUnit) {
  return UserExpression::Builder(Internal::Angle::Period(angleUnit));
}

template std::complex<float> NewTrigonometry::ConvertToRadian<float>(
    std::complex<float>, Preferences::AngleUnit);
template std::complex<double> NewTrigonometry::ConvertToRadian<double>(
    std::complex<double>, Preferences::AngleUnit);
template std::complex<float> NewTrigonometry::ConvertRadianToAngleUnit<float>(
    std::complex<float>, Preferences::AngleUnit);
template std::complex<double> NewTrigonometry::ConvertRadianToAngleUnit<double>(
    std::complex<double>, Preferences::AngleUnit);

}  // namespace Poincare
