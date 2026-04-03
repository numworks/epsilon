#pragma once

#include <poincare/src/expression/continuity.h>

namespace Poincare::NumericSolver {

template <typename TFloat>
struct NullDiscontinuityTester {
  template <typename TEvaluator>
  static bool IsDiscontinuousBetween(Coordinate2D<TFloat>, Coordinate2D<TFloat>,
                                     const TEvaluator&) {
    return false;
  }
};

template <typename TFloat>
struct ExpressionDiscontinuityTester {
  template <typename TEvaluator>
  static bool IsDiscontinuousBetween(Coordinate2D<TFloat> a,
                                     Coordinate2D<TFloat> b,
                                     const TEvaluator& eval) {
    return (std::isfinite(a.y()) && std::isnan(b.y())) ||
           (std::isfinite(b.y()) && std::isnan(a.y())) ||
           Internal::Continuity::IsDiscontinuousOnInterval<TFloat>(
               eval.tree(), a.x(), b.x());
  }
};

}  // namespace Poincare::NumericSolver
