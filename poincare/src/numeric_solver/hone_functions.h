#pragma once

#include <omg/unreachable.h>
#include <poincare/numeric_solver/policies/evaluation.h>

namespace Poincare::Internal {

class HoneFunctions {
 public:
  // Hone methods
  static Coordinate2D<double> SafeBrentMinimum(
      const NumericSolver::LambdaEvaluator<double>&, double xMin, double xMax,
      Interest, double xPrecision);

  static Coordinate2D<double> SafeBrentMaximum(
      const NumericSolver::LambdaEvaluator<double>&, double xMin, double xMax,
      Interest, double xPrecision);

  static Coordinate2D<double> SafeBrentExtremum(
      const NumericSolver::LambdaEvaluator<double>&, double xMin, double xMax,
      Interest, double xPrecision);

  static Coordinate2D<double> CompositeBrentForRoot(
      const NumericSolver::LambdaEvaluator<double>&, double xMin, double xMax,
      Interest, double xPrecision);

  static Coordinate2D<double> DummyHone(
      const NumericSolver::LambdaEvaluator<double>&, double, double, Interest,
      double) {
    OMG_UNREACHABLE;
  }
};

}  // namespace Poincare::Internal
