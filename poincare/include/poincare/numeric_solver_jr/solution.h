#pragma once

#include <poincare/coordinate_2D.h>
#include <poincare/numeric_solver/interest.h>

namespace Poincare::NumericSolver {

template <typename T>
struct Solution {
  T x = static_cast<T>(NAN);
  T y = static_cast<T>(NAN);
  Interest interest = Interest::None;
};

template <typename T>
constexpr inline Solution<T> CoordToSolution(
    Coordinate2D<T> coordinates, Interest interest = Interest::None) {
  return Solution<T>{coordinates.x(), coordinates.y(), interest};
}

template <typename T>
constexpr inline Coordinate2D<T> SolutionToCoord(Solution<T> solution) {
  return Coordinate2D<T>(solution.x, solution.y);
}

}  // namespace Poincare::NumericSolver
