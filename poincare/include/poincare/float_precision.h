#pragma once

namespace Poincare {

// Precision of floating-point calculations
enum class FloatPrecision { Single, Double };
/* For a given function, if its body depends on the float type (for example if a
 * float / double variable is declared), the function has to be templated with
 the float type. However if the float type is not used in the function body, the
 function should not be templated, to avoid a duplicate template instantiation
 and gain binary size. To forward the float precision information to other
 functions, a FloatPrecision parameter is passed as a function argument. */

/* Helper struct to convert a float template type into a FloatPrecision value.
 * Usage: Precision<T>::value.
 * Note that the reverse operation is not possible. If FloatPrecision is passed
 * as a function parameter, its value is not known at compile-time. So we are
 * forced to conditionally call a "float" or "double" version of a template
 * function depending on the FloatPrecision value. */
template <typename FloatType>
struct Precision {};
template <>
struct Precision<float> {
  constexpr static FloatPrecision value = FloatPrecision::Single;
};
template <>
struct Precision<double> {
  constexpr static FloatPrecision value = FloatPrecision::Double;
};

/* Float type used where single precision would normally suffice. When float
 * approximation is compiled out, it collapses to double so no single-float code
 * (and no soft-float runtime) is emitted; otherwise it stays float. */
#if POINCARE_NO_FLOAT_APPROXIMATION
using SmallerFloat = double;
#else
using SmallerFloat = float;
#endif

}  // namespace Poincare
