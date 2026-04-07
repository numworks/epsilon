#pragma once

#include <poincare/src/expression/continuity.h>

namespace Poincare::NumericSolver {

template <typename TFloat>
class NullDiscontinuityTester {
 public:
  bool isDiscontinuousBetween(Coordinate2D<TFloat>, Coordinate2D<TFloat>,
                              const void*) const {
    return false;
  }
};

template <typename TFloat>
class ExpressionDiscontinuityTester {
 public:
  bool isDiscontinuousBetween(Coordinate2D<TFloat> a, Coordinate2D<TFloat> b,
                              const void* aux) const {
    const Internal::Tree* tree = static_cast<const Internal::Tree*>(aux);
    return (std::isfinite(a.y()) && std::isnan(b.y())) ||
           (std::isfinite(b.y()) && std::isnan(a.y())) ||
           Internal::Continuity::IsDiscontinuousOnInterval<TFloat>(tree, a.x(),
                                                                   b.x());
  }
};

template <typename TFloat>
struct LambdaDiscontinuityTester {
  using DiscontinuityTest = bool (*)(Coordinate2D<TFloat>, Coordinate2D<TFloat>,
                                     const void*);

 public:
  LambdaDiscontinuityTester(DiscontinuityTest test) : m_test(test) {}

  /* NOTE: Conversion function to turn other, static testers into a
   * LambdaDiscontinuityTester. */
  template <typename TTester>
  LambdaDiscontinuityTester(const TTester&)
      : m_test([](Coordinate2D<TFloat> a, Coordinate2D<TFloat> b,
                  const void* aux) {
          return TTester{}.isDiscontinuousBetween(a, b, aux);
        }) {}

  bool isDiscontinuousBetween(Coordinate2D<TFloat> a, Coordinate2D<TFloat> b,
                              const void* aux) const {
    return m_test(a, b, aux);
  }

 private:
  DiscontinuityTest m_test;
};

}  // namespace Poincare::NumericSolver
