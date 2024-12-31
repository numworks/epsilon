#ifndef POINCARE_TRIGONOMETRY_H
#define POINCARE_TRIGONOMETRY_H

#include <complex.h>
#include <poincare/preferences.h>

namespace Poincare {

namespace Trigonometry {
double PiInAngleUnit(Preferences::AngleUnit angleUnit);
double ConvertAngleToRadian(double angle, Preferences::AngleUnit angleUnit);

template <typename T>
std::complex<T> ConvertToRadian(const std::complex<T> c,
                                Preferences::AngleUnit angleUnit);
template <typename T>
std::complex<T> ConvertRadianToAngleUnit(const std::complex<T> c,
                                         Preferences::AngleUnit angleUnit);
};  // namespace Trigonometry

}  // namespace Poincare

#endif
