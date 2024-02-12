#ifndef POINCARE_JUNIOR_NUMERIC_FLOAT_H
#define POINCARE_JUNIOR_NUMERIC_FLOAT_H

#include <float.h>

namespace PoincareJ {

template <typename T>
class Float {
 public:
  constexpr static T EpsilonLax();
  constexpr static T Epsilon();
  constexpr static T SqrtEpsilonLax();
  constexpr static T Min();
  constexpr static T Max();
};

/* To prevent incorrect approximations, such as cos(1.5707963267949) = 0
 * we made the neglect threshold stricter. This way, the approximation is more
 * selective.
 * However, when ploting functions such as e^(i.pi+x), the float approximation
 * fails by giving non-real results and therefore, the function appears "undef".
 * As a result we created two functions Epsilon that behave differently
 * according to the number's type. When it is a double we want maximal precision
 * -> precision_double = 1x10^(-15).
 * When it is a float, we accept more agressive approximations
 * -> precision_float = x10^(-6). */

template <>
constexpr inline float Float<float>::EpsilonLax() {
  return 1E-6f;
}
template <>
constexpr inline double Float<double>::EpsilonLax() {
  return 1E-15;
}
template <>
constexpr inline float Float<float>::Epsilon() {
  return FLT_EPSILON;
}
template <>
constexpr inline double Float<double>::Epsilon() {
  return DBL_EPSILON;
}
template <>
constexpr inline float Float<float>::SqrtEpsilonLax() {
  return 1e-3f;
}
template <>
constexpr inline double Float<double>::SqrtEpsilonLax() {
  return 3e-8f;
}
template <>
constexpr inline float Float<float>::Min() {
  return FLT_MIN;
}
template <>
constexpr inline double Float<double>::Min() {
  return DBL_MIN;
}
template <>
constexpr inline float Float<float>::Max() {
  return FLT_MAX;
}
template <>
constexpr inline double Float<double>::Max() {
  return DBL_MAX;
}

}  // namespace PoincareJ

#endif
