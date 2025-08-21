#include <poincare/src/expression/angle.h>
#include <poincare/trigonometry.h>

namespace Poincare {

double Trigonometry::PiInAngleUnit(AngleUnit angleUnit) {
  assert(angleUnit != AngleUnit::None);
  switch (angleUnit) {
    case AngleUnit::Radian:
      return M_PI;
    case AngleUnit::Degree:
      return 180.0;
    default:
      assert(angleUnit == AngleUnit::Gradian);
      return 200.0;
  }
}

double Trigonometry::ConvertAngleToRadian(double angle, AngleUnit angleUnit) {
  return angleUnit != AngleUnit::Radian
             ? angle * M_PI / PiInAngleUnit(angleUnit)
             : angle;
}

template <typename T>
std::complex<T> Trigonometry::ConvertToRadian(const std::complex<T> c,
                                              AngleUnit angleUnit) {
  if (angleUnit != AngleUnit::Radian) {
    return c * std::complex<T>((T)M_PI / (T)PiInAngleUnit(angleUnit));
  }
  return c;
}

template <typename T>
std::complex<T> Trigonometry::ConvertRadianToAngleUnit(const std::complex<T> c,
                                                       AngleUnit angleUnit) {
  if (angleUnit != AngleUnit::Radian) {
    return c * std::complex<T>((T)PiInAngleUnit(angleUnit) / (T)M_PI);
  }
  return c;
}

template std::complex<float> Trigonometry::ConvertToRadian<float>(
    std::complex<float>, AngleUnit);
template std::complex<double> Trigonometry::ConvertToRadian<double>(
    std::complex<double>, AngleUnit);
template std::complex<float> Trigonometry::ConvertRadianToAngleUnit<float>(
    std::complex<float>, AngleUnit);
template std::complex<double> Trigonometry::ConvertRadianToAngleUnit<double>(
    std::complex<double>, AngleUnit);

}  // namespace Poincare
