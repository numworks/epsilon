#pragma once

#include <poincare/numeric_solver_jr/solver.h>

namespace Poincare::Internal {

template <typename T>
class HoneFunctions {
 public:
  // Hone methods
  static Coordinate2D<T> SafeBrentMinimum(
      typename NumericSolver::Solver<T>::FunctionEvaluation f, const void* aux,
      T xMin, T xMax, Interest interest, T xPrecision);

  static Coordinate2D<T> SafeBrentMaximum(
      typename NumericSolver::Solver<T>::FunctionEvaluation f, const void* aux,
      T xMin, T xMax, Interest interest, T xPrecision);

  static Coordinate2D<T> CompositeBrentForRoot(
      typename NumericSolver::Solver<T>::FunctionEvaluation f, const void* aux,
      T xMin, T xMax, Interest interest, T xPrecision);

  static Coordinate2D<T> DummyHone(
      typename NumericSolver::Solver<T>::FunctionEvaluation, const void*, T, T,
      Interest, T) {
    assert(false);
    return Coordinate2D<T>();
  }
};

}  // namespace Poincare::Internal
