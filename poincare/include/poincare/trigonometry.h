#ifndef POINCARE_TRIGONOMETRY_H
#define POINCARE_TRIGONOMETRY_H

#include <complex.h>
#include <poincare/preferences.h>

namespace Poincare {

namespace Trigonometry {
double PiInAngleUnit(AngleUnit angleUnit);
double ConvertAngleToRadian(double angle, AngleUnit angleUnit);

template <typename T>
std::complex<T> ConvertToRadian(const std::complex<T> c, AngleUnit angleUnit);
template <typename T>
std::complex<T> ConvertRadianToAngleUnit(const std::complex<T> c,
                                         AngleUnit angleUnit);
};  // namespace Trigonometry

}  // namespace Poincare

#endif
