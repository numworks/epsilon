#ifndef POINCARE_SOLVER_ALGORITHMS_H
#define POINCARE_SOLVER_ALGORITHMS_H

#include <omg/float.h>

#include "helpers.h"
#include "solver.h"

namespace Poincare {

class OSolverAlgorithms {
 public:
  constexpr static double k_goldenRatio =
      0.381966011250105151795413165634361882279690820194237137864;  // (3-sqrt(5))/2

  /* Bibliography: R. P. Brent, Algorithms for finding zeros and extrema of
   * functions without calculating derivatives
   *
   * These two functions must be of the OSolver<double>::HoneResult type.
   * Users who want to find roots and extrema with good precision should
   * use the OSolver<double> API. */
  static Coordinate2D<double> BrentRoot(OSolver<double>::FunctionEvaluation f,
                                        const void* aux, double xMin,
                                        double xMax,
                                        OSolver<double>::Interest interest,
                                        double precision);
  static Coordinate2D<double> BrentMinimum(
      OSolver<double>::FunctionEvaluation f, const void* aux, double xMin,
      double xMax, OSolver<double>::Interest interest, double precision);

  /* Numeric functions for statistics and probability. */
  static Coordinate2D<double> IncreasingFunctionRoot(
      double ax, double bx, double resultPrecision,
      OSolver<double>::FunctionEvaluation f, const void* aux,
      double* resultEvaluation = nullptr);
  template <typename T>
  static T CumulativeDistributiveInverseForNDefinedFunction(
      T* probability, typename OSolver<T>::FunctionEvaluation f,
      const void* aux);
  template <typename T>
  static T CumulativeDistributiveFunctionForNDefinedFunction(
      T x, typename OSolver<T>::FunctionEvaluation f, const void* aux);

 private:
  constexpr static int k_numberOfIterationsBrent = 100;
  constexpr static double k_sqrtEps =
      OMG::Float::SquareRoot<double>(OMG::Float::Epsilon<double>());
  static_assert(k_sqrtEps == 1.4901161193847656E-8,
                "Wrong value for sqrt(DBL_EPSILON");
  constexpr static int k_numberOfIterationsProbability = 1000000;
  constexpr static double k_maxProbability = 0.9999995;
};

}  // namespace Poincare

#endif
