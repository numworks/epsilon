#pragma once

#include <omg/unreachable.h>
#include <poincare/numeric_solver/policies/evaluation.h>
#include <poincare/numeric_solver/solver.h>

namespace Poincare::Internal {

class HoneFunctions {
 public:
  // Hone methods
  static Coordinate2D<double> SafeBrentMinimum(
      typename NumericSolver::FunctionEvaluation<double> f, const void* aux,
      double xMin, double xMax, Interest interest, double xPrecision);

  static Coordinate2D<double> SafeBrentMaximum(
      typename NumericSolver::FunctionEvaluation<double> f, const void* aux,
      double xMin, double xMax, Interest interest, double xPrecision);

  static Coordinate2D<double> SafeBrentExtremum(
      typename NumericSolver::FunctionEvaluation<double> f, const void* aux,
      double xMin, double xMax, Interest interest, double xPrecision);

  static Coordinate2D<double> CompositeBrentForRoot(
      typename NumericSolver::FunctionEvaluation<double> f, const void* aux,
      double xMin, double xMax, Interest interest, double xPrecision);

  static Coordinate2D<double> DummyHone(
      typename NumericSolver::FunctionEvaluation<double>, const void*, double,
      double, Interest, double) {
    OMG_UNREACHABLE;
  }
};

}  // namespace Poincare::Internal
