#pragma once

#include <poincare/preferences.h>

#include <complex>

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
