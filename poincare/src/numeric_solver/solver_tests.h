#pragma once

#include <omg/float.h>
#include <poincare/coordinate_2D.h>
#include <poincare/numeric_solver/interest.h>
#include <poincare/src/expression/continuity.h>

namespace Poincare::Internal {

template <typename T>
class SolverTests {
 public:
  // BracketTest default implementations
  static Interest OddRootInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                   Coordinate2D<T> c, const void*) {
    return BoolToInterest(HasOddRoot(a, b, c), Interest::Root);
  }

  static Interest MinimumInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                   Coordinate2D<T> c, const void*) {
    return BoolToInterest(HasMinimum(a, b, c), Interest::Minimum);
  }

  static Interest MaximumInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                   Coordinate2D<T> c, const void*) {
    return BoolToInterest(HasMaximum(a, b, c), Interest::Maximum);
  }

  static Interest EvenOrOddRootInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                         Coordinate2D<T> c, const void* aux) {
    Interest interest = Interest::None;
    if ((interest = OddRootInBracket(a, b, c, aux)) != Interest::None) {
      return interest;
    }
    /* FIXME Check the sign of a,b and c. A minimum can only be a root if b is
     * positive. */
    if ((interest = MinimumInBracket(a, b, c, aux)) != Interest::None) {
      return interest;
    }
    return MaximumInBracket(a, b, c, aux);
  }

  static bool IsDiscontinuousInBracket(Coordinate2D<T> a, Coordinate2D<T> c,
                                       const void* aux) {
    return HasDiscontinuityAtEdge(a, c) ||
           Continuity::IsDiscontinuousOnInterval<T>(
               reinterpret_cast<const Internal::Tree*>(aux), a.x(), c.x());
  }

  static Interest DiscontinuityAtEdge(Coordinate2D<T> a, Coordinate2D<T> b,
                                      Coordinate2D<T> c, const void* aux) {
    return BoolToInterest(HasDiscontinuityAtEdge(a, c),
                          Interest::Discontinuity);
  }

  static Interest DiscontinuityInBracket(Coordinate2D<T> a, Coordinate2D<T> b,
                                         Coordinate2D<T> c, const void* aux) {
    return BoolToInterest(IsDiscontinuousInBracket(a, c, aux),
                          Interest::ReachedDiscontinuity);
  }

 private:
  constexpr static Interest BoolToInterest(bool v, Interest t) {
    return v ? t : Interest::None;
  }

  static bool HasOddRoot(Coordinate2D<T> a, Coordinate2D<T> b,
                         Coordinate2D<T> c) {
    return (a.y() < 0. && 0. < c.y()) || (c.y() < 0. && 0. < a.y());
  }

  static bool HasMinimum(Coordinate2D<T> a, Coordinate2D<T> b,
                         Coordinate2D<T> c) {
    return (b.y() < a.y() && b.y() < c.y());
  }

  static bool HasMaximum(Coordinate2D<T> a, Coordinate2D<T> b,
                         Coordinate2D<T> c) {
    return (a.y() < b.y() && c.y() < b.y());
  }

  static bool HasDiscontinuityAtEdge(Coordinate2D<T> a, Coordinate2D<T> b) {
    return (std::isfinite(a.y()) && std::isnan(b.y())) ||
           (std::isfinite(b.y()) && std::isnan(a.y()));
  }
};

}  // namespace Poincare::Internal
