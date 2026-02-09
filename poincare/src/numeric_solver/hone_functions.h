#pragma once

#include <poincare/numeric_solver/solver.h>

namespace Poincare::Internal {

class HoneFunctions {
 public:
  // Hone methods
  static Coordinate2D<double> SafeBrentMinimum(
      typename NumericSolver::Solver<double>::FunctionEvaluation f,
      const void* aux, double xMin, double xMax, Interest interest,
      double xPrecision);

  static Coordinate2D<double> SafeBrentMaximum(
      typename NumericSolver::Solver<double>::FunctionEvaluation f,
      const void* aux, double xMin, double xMax, Interest interest,
      double xPrecision);

  static Coordinate2D<double> SafeBrentExtremum(
      typename NumericSolver::Solver<double>::FunctionEvaluation f,
      const void* aux, double xMin, double xMax, Interest interest,
      double xPrecision);

  static Coordinate2D<double> CompositeBrentForRoot(
      typename NumericSolver::Solver<double>::FunctionEvaluation f,
      const void* aux, double xMin, double xMax, Interest interest,
      double xPrecision);

  static Coordinate2D<double> DummyHone(
      typename NumericSolver::Solver<double>::FunctionEvaluation, const void*,
      double, double, Interest, double) {
    assert(false);
    return Coordinate2D<double>();
  }
};

}  // namespace Poincare::Internal
