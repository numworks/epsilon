#pragma once

#include <omg/float.h>
#include <poincare/coordinate_2D.h>
#include <poincare/numeric_solver/policies/evaluation.h>
#include <poincare/numeric_solver/solution.h>

namespace Poincare::Internal {

class SolverAlgorithms {
 public:
  constexpr static double k_goldenRatio =
      0.381966011250105151795413165634361882279690820194237137864;  // (3-sqrt(5))/2

  /* Bibliography: R. P. Brent, Algorithms for finding zeros and extrema of
   * functions without calculating derivatives */
  static Coordinate2D<double> BrentRoot(
      const NumericSolver::LambdaEvaluator<double>& f, double xMin, double xMax,
      Interest interest, double xPrecision);
  static Coordinate2D<double> BrentMinimum(
      const NumericSolver::LambdaEvaluator<double>& f, double xMin, double xMax,
      Interest interest, double xPrecision);

  /* Numeric functions for statistics and probability. */
  static Coordinate2D<double> IncreasingFunctionRoot(
      double ax, double bx, double xPrecision,
      const NumericSolver::LambdaEvaluator<double>& f,
      double* resultEvaluation = nullptr);

  template <typename T>
  static T CumulativeDistributiveInverseForNDefinedFunction(
      T* probability, const NumericSolver::LambdaEvaluator<T>& f);
  template <typename T>
  static T CumulativeDistributiveFunctionForNDefinedFunction(
      T x, const NumericSolver::LambdaEvaluator<T>& f);

 private:
  constexpr static int k_numberOfIterationsBrent = 100;
  constexpr static double k_sqrtEps = OMG::Float::SqrtEpsilon<double>();
  static_assert(k_sqrtEps == 1.4901161193847656E-8,
                "Wrong value for sqrt(DBL_EPSILON");
  constexpr static int k_numberOfIterationsProbability = 1000000;
  constexpr static double k_maxProbability = 0.9999995;
};

}  // namespace Poincare::Internal
