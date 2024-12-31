#include <poincare/src/expression/angle.h>
#include <poincare/trigonometry.h>

namespace Poincare {

double Trigonometry::PiInAngleUnit(Preferences::AngleUnit angleUnit) {
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

double Trigonometry::ConvertAngleToRadian(double angle,
                                          Preferences::AngleUnit angleUnit) {
  return angleUnit != Preferences::AngleUnit::Radian
             ? angle * M_PI / PiInAngleUnit(angleUnit)
             : angle;
}

template <typename T>
std::complex<T> Trigonometry::ConvertToRadian(
    const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (angleUnit != Preferences::AngleUnit::Radian) {
    return c * std::complex<T>((T)M_PI / (T)PiInAngleUnit(angleUnit));
  }
  return c;
}

template <typename T>
std::complex<T> Trigonometry::ConvertRadianToAngleUnit(
    const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  if (angleUnit != Preferences::AngleUnit::Radian) {
    return c * std::complex<T>((T)PiInAngleUnit(angleUnit) / (T)M_PI);
  }
  return c;
}

template std::complex<float> Trigonometry::ConvertToRadian<float>(
    std::complex<float>, Preferences::AngleUnit);
template std::complex<double> Trigonometry::ConvertToRadian<double>(
    std::complex<double>, Preferences::AngleUnit);
template std::complex<float> Trigonometry::ConvertRadianToAngleUnit<float>(
    std::complex<float>, Preferences::AngleUnit);
template std::complex<double> Trigonometry::ConvertRadianToAngleUnit<double>(
    std::complex<double>, Preferences::AngleUnit);

}  // namespace Poincare
