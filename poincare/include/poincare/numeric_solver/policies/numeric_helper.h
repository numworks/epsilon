#pragma once

#include <omg/float.h>

namespace Poincare::NumericSolver {

template <typename TFloat>
struct DefaultNumericHelper {
  constexpr static TFloat k_relativePrecision = OMG::Float::Epsilon<TFloat>();
  constexpr static TFloat k_sqrtRelativePrecision =
      OMG::Float::SqrtEpsilon<TFloat>();
  constexpr static TFloat k_minimalAbsoluteStep =
      static_cast<TFloat>(2.) *
      OMG::Float::SquareRoot<TFloat>(static_cast<TFloat>(2.) *
                                     k_relativePrecision);
  constexpr static TFloat k_minimalPracticalStep =
      std::max(static_cast<TFloat>(1e-6), k_minimalAbsoluteStep);

  static TFloat NullTolerance(TFloat x) {
    return std::max(k_relativePrecision,
                    k_sqrtRelativePrecision * std::fabs(x));
  }
  static TFloat MagicRound(TFloat x) {
    constexpr TFloat k_roundingOrder =
        static_cast<TFloat>(2.) * k_minimalPracticalStep;
    return k_roundingOrder * std::round(x / k_roundingOrder);
  }
  static TFloat Precision(TFloat x, bool isDiscontinuous) {
    constexpr TFloat k_discPrecision =
        k_relativePrecision * k_minimalAbsoluteStep;
    return isDiscontinuous ? k_discPrecision : NullTolerance(x);
  }
  static TFloat CleanY(TFloat y, TFloat x) {
    return std::fabs(y) < NullTolerance(x) ? static_cast<TFloat>(0) : y;
  }
};

}  // namespace Poincare::NumericSolver
